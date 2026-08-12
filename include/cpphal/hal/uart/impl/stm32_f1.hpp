#pragma once

#include <device.hpp>

#include "configurator.hpp"
#include "driver.hpp"
#include "hal/core/event.hpp"

namespace hal::uart::impl {
template <class... Fields>
struct encode {
  static constexpr auto value = (Fields::encode(1) | ...);
};

template <class List>
struct encode_or_zero {
  using type = meta::mp_apply<encode, List>;
};

template <>
struct encode_or_zero<void> {
  using type = std::integral_constant<std::uint32_t, 0>;
};

template <
  class Peripheral,
  class BasicConfig,
  class AdvancedConfig>
struct Configurator<mcu::policy::STM32F1Policy, Peripheral, BasicConfig, AdvancedConfig> {
  // private:
  template <std::uint32_t Pclk, std::uint32_t Baud>
  struct BRR {
    static constexpr std::uint32_t usartdiv16 = (Pclk + Baud / 2) / Baud;
    static constexpr std::uint32_t mantissa   = usartdiv16 / 16;
    static constexpr std::uint32_t fraction   = usartdiv16 % 16;
    static constexpr std::uint32_t value      = (mantissa << 4) | fraction;
  };

  struct get_register {
    template <class T>
    using fn = T::register_type;
  };

  struct get_bit {
    template <class T>
    using fn = T;
  };

  using basic = Basic<Peripheral, BasicConfig>;

  using advanced = Advanced<Peripheral, AdvancedConfig>;

  using events_registers = meta::group_by<get_register, get_bit, typename advanced::enables::value>;

  template <std::uint32_t TE, std::uint32_t RE>
  struct DirectionBits {
    static constexpr std::uint32_t te = TE;
    static constexpr std::uint32_t re = RE;
  };

  template <std::uint32_t Cts, std::uint32_t Rts>
  struct FlowControlBits {
    static constexpr std::uint32_t cts = Cts;
    static constexpr std::uint32_t rts = Rts;
  };

  using DirectionMap = meta::mp_list<
    meta::mp_list<options::direction::Tx, DirectionBits<1, 0>>,
    meta::mp_list<options::direction::Rx, DirectionBits<0, 1>>,
    meta::mp_list<options::direction::TxRx, DirectionBits<1, 1>>
  >;

  using FlowControlMap = meta::mp_list<
    meta::mp_list<options::flow_control::None, FlowControlBits<0, 0>>,
    meta::mp_list<options::flow_control::Cts, FlowControlBits<1, 0>>,
    meta::mp_list<options::flow_control::Rts, FlowControlBits<0, 1>>,
    meta::mp_list<options::flow_control::RtsCts, FlowControlBits<1, 1>>
  >;

  using StopBitsMap = meta::mp_list<
    meta::mp_list<options::StopBits<1>, options::StopBits<0b00>>,
    meta::mp_list<options::StopBits<0.5>, options::StopBits<0b01>>,
    meta::mp_list<options::StopBits<2>, options::StopBits<0b10>>,
    meta::mp_list<options::StopBits<1.5>, options::StopBits<0b11>>
  >;

  template <class List, class Reg>
  using encode_t = encode_or_zero<meta::get_by_key_t<List, Reg>>::type;

  template <class Reg>
  using Fallback = std::integral_constant<typename Reg::value_type, 0>;

  template <class M>
  using flow_control = option_resolver_t<
    M,
    typename advanced::flowcontrol,
    options::flow_control::None
  >;

  static consteval std::uint32_t cr1_bits() {
    using cr1_events         = encode_t<events_registers, typename Peripheral::CR1>;
    constexpr auto cr1_value = cr1_events::value;

    using direction_bits_pair = meta::mp_map_find<DirectionMap, typename basic::direction>;
    static_assert(!std::is_same_v<direction_bits_pair, void>, "Unsupported Direction");
    using direction_bits = meta::mp_second<direction_bits_pair>;

    return Peripheral::CR1::PCE::encode(std::is_same_v<typename basic::parity, options::parity::None> ? 0 : 1)
           | Peripheral::CR1::PS::encode(std::is_same_v<typename basic::parity, options::parity::Even> ? 0 : 1)
           | Peripheral::CR1::M::encode(basic::databits::value == 8 ? 0 : 1)
           | Peripheral::CR1::TE::encode(direction_bits::te)
           | Peripheral::CR1::RE::encode(direction_bits::re)
           | Peripheral::CR1::UE::encode(1)
           | cr1_value;
  }

  static consteval std::uint32_t cr2_bits() {
    using cr2_events         = encode_t<events_registers, typename Peripheral::CR2>;
    constexpr auto cr2_value = cr2_events::value;

    using stop_bits_pair = meta::mp_map_find<StopBitsMap, typename basic::stopbits>;
    static_assert(!std::is_same_v<stop_bits_pair, void>, "Unsupported StopBits");
    using stop_bits = meta::mp_second<stop_bits_pair>;

    return Peripheral::CR2::STOP::encode(stop_bits::value)
           | cr2_value;
  }

  static consteval std::uint32_t cr3_bits() {
    using cr3_events         = encode_t<events_registers, typename Peripheral::CR3>;
    constexpr auto cr3_value = cr3_events::value;

    using flowcontrol_bits_pair = flow_control<FlowControlMap>;
    static_assert(!std::is_same_v<flowcontrol_bits_pair, void>, "Unsupported FlowControl");
    using flowcontrol_bits = meta::mp_second<flowcontrol_bits_pair>;

    return Peripheral::CR3::CTSE::encode(flowcontrol_bits::cts)
           | Peripheral::CR3::RTSE::encode(flowcontrol_bits::rts)
           | cr3_value;
  }

public:
  template <class ClockConfig>
  static void apply() {
    using clock_bus = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    Peripheral::BRR::write(BRR<clock_bus::frequency, basic::baud::value>::value);

    constexpr auto cr3 = cr3_bits();
    if constexpr (cr3 != 0) Peripheral::CR3::write(cr3);

    constexpr auto cr2 = cr2_bits();
    if constexpr (cr2 != 0) Peripheral::CR2::write(cr2);

    constexpr auto cr1 = cr1_bits();
    if constexpr (cr1 != 0) Peripheral::CR1::write(cr1);
  }
};

template <
  class Peripheral,
  class BasicConfig,
  class AdvancedConfig>
struct Driver<mcu::policy::STM32F1Policy, Peripheral, BasicConfig, AdvancedConfig> {
private:
  using basic     = Basic<Peripheral, BasicConfig>;
  using direction = basic::direction;

public:
  template <class T, class D = direction>
  static void write(const T& c)
    requires std::same_as<D, options::direction::Tx> || std::same_as<D, options::direction::TxRx> {
    write<D>(static_cast<uint8_t*>(&c), sizeof(T));
  }

  template <class D = direction>
  static void write(const char* str)
    requires std::same_as<D, options::direction::Tx> || std::same_as<D, options::direction::TxRx> {
    write<D>(reinterpret_cast<const uint8_t*>(str), strlen(str));
  }

  template <class D = direction>
  static void write(const uint8_t* data, uint32_t size)
    requires std::same_as<D, options::direction::Tx> || std::same_as<D, options::direction::TxRx> {
    while (size-- > 0) {
      while (!Peripheral::SR::TXE::read()) { Peripheral::DR::write(*data++); }
    }
  }
};
}