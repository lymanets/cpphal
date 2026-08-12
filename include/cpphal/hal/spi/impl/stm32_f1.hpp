#pragma once

#include <device.hpp>

#include "configurator.hpp"
#include "driver.hpp"
#include "hal/spi/options.hpp"

namespace hal::spi::impl {
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

  // template <std::uint32_t TE, std::uint32_t RE>
  // struct DirectionBits {
  //   static constexpr std::uint32_t te = TE;
  //   static constexpr std::uint32_t re = RE;
  // };
  //
  // template <std::uint32_t Cts, std::uint32_t Rts>
  // struct FlowControlBits {
  //   static constexpr std::uint32_t cts = Cts;
  //   static constexpr std::uint32_t rts = Rts;
  // };

  // using DirectionMap = meta::mp_list<
  //   meta::mp_list<options::direction::Tx, DirectionBits<1, 0>>,
  //   meta::mp_list<options::direction::Rx, DirectionBits<0, 1>>,
  //   meta::mp_list<options::direction::TxRx, DirectionBits<1, 1>>
  // >;
  //
  // using FlowControlMap = meta::mp_list<
  //   meta::mp_list<options::flow_control::None, FlowControlBits<0, 0>>,
  //   meta::mp_list<options::flow_control::Cts, FlowControlBits<1, 0>>,
  //   meta::mp_list<options::flow_control::Rts, FlowControlBits<0, 1>>,
  //   meta::mp_list<options::flow_control::RtsCts, FlowControlBits<1, 1>>
  // >;
  //
  // using StopBitsMap = meta::mp_list<
  //   meta::mp_list<options::StopBits<1>, options::StopBits<0b00>>,
  //   meta::mp_list<options::StopBits<0.5>, options::StopBits<0b01>>,
  //   meta::mp_list<options::StopBits<2>, options::StopBits<0b10>>,
  //   meta::mp_list<options::StopBits<1.5>, options::StopBits<0b11>>
  // >;

  template <class List, class Reg>
  using encode_t = encode_or_zero<meta::get_by_key_t<List, Reg>>::type;

  template <class Reg>
  using Fallback = std::integral_constant<typename Reg::value_type, 0>;

  static consteval std::uint32_t cr1_bits() {
    using direction = typename basic::direction;

    constexpr auto rxonly   = std::is_same_v<direction, options::direction::ReceiveOnly>;
    constexpr auto bidimode = std::is_same_v<direction, options::direction::HalfDuplexTransmit>
                              || std::is_same_v<direction, options::direction::HalfDuplexReceive>;
    constexpr auto bidioe   = std::is_same_v<direction, options::direction::HalfDuplexTransmit>;
    constexpr auto lsbfirst = std::is_same_v<typename basic::bit_order, options::bit_order::LSBFirst>;
    constexpr auto mstr     = std::is_same_v<typename basic::mode, options::mode::Master>;
    constexpr auto cpol     = std::is_same_v<typename basic::clock_polarity, options::clock_polarity::High>;
    constexpr auto cpha     = std::is_same_v<typename basic::clock_phase, options::clock_phase::SecondEdge>;
    constexpr auto dff      = basic::data_bits::value == 16;

    using cr1_events = encode_t<events_registers, typename Peripheral::CR1>;

    return Peripheral::CR1::RXONLY::encode(rxonly)
           | Peripheral::CR1::BIDIMODE::encode(bidimode)
           | Peripheral::CR1::BIDIOE::encode(bidioe)
           | Peripheral::CR1::DFF::encode(dff)
           | Peripheral::CR1::LSBFIRST::encode(lsbfirst)
           | Peripheral::CR1::SPE::encode(1)
           | Peripheral::CR1::MSTR::encode(mstr)
           | Peripheral::CR1::CPOL::encode(cpol)
           | Peripheral::CR1::CPHA::encode(cpha)
           | cr1_events::value;
  }

  static consteval std::uint32_t cr2_bits() {
    using cr2_events         = encode_t<events_registers, typename Peripheral::CR2>;
    constexpr auto cr2_value = cr2_events::value;

    return cr2_value;
  }

public:
  template <class ClockConfig>
  static void apply() {
    using clock_bus = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    // Peripheral::BRR::write(BRR<clock_bus::frequency, basic::baud::value>::value);

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
  using basic = Basic<Peripheral, BasicConfig>;

public:
  template <class T>
  static void write(const T& c) {
    write(static_cast<uint8_t*>(&c), sizeof(T));
  }

  static void write(const char* str) {
    write(reinterpret_cast<const uint8_t*>(str), strlen(str));
  }

  static void write(const uint8_t* data, uint32_t size) {
    while (size-- > 0) {
      while (!Peripheral::SR::TXE::read()) { Peripheral::DR::write(*data++); }
    }
  }
};
}