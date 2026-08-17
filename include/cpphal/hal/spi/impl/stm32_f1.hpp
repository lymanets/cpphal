#pragma once

#include <bit>
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
  template <std::uint32_t Pclk, std::uint32_t Requested>
  struct Prescaler {
    static_assert(Requested <= Pclk / 2, "SPI frequency is too high");

    static constexpr std::uint32_t value =
        (Requested >= Pclk / 2)
          ? 2
          : (Requested >= Pclk / 4)
          ? 4
          : (Requested >= Pclk / 8)
          ? 8
          : (Requested >= Pclk / 16)
          ? 16
          : (Requested >= Pclk / 32)
          ? 32
          : (Requested >= Pclk / 64)
          ? 64
          : (Requested >= Pclk / 128)
          ? 128
          : 256;

    static constexpr std::uint32_t br = std::countr_zero(value) - 1;
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

  template <class List, class Reg>
  using encode_t = encode_or_zero<meta::get_by_key_t<List, Reg>>::type;

  template <class Reg>
  using Fallback = std::integral_constant<typename Reg::value_type, 0>;

  template <class ClockConfig>
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

    using clock_bus = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    using br        = Prescaler<clock_bus::frequency, basic::baud::value>;

    return Peripheral::CR1::RXONLY::encode(rxonly)
           | Peripheral::CR1::BIDIMODE::encode(bidimode)
           | Peripheral::CR1::BIDIOE::encode(bidioe)
           | Peripheral::CR1::DFF::encode(dff)
           | Peripheral::CR1::LSBFIRST::encode(lsbfirst)
           | Peripheral::CR1::SPE::encode(1)
           | Peripheral::CR1::SSM::encode(1)
           | Peripheral::CR1::SSI::encode(1)
           | Peripheral::CR1::BR::encode(br::br)
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
    constexpr auto cr2 = cr2_bits();
    if constexpr (cr2 != 0) Peripheral::CR2::write(cr2);

    constexpr auto cr1 = cr1_bits<ClockConfig>();
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
  static uint8_t transfer(const uint8_t data) {
    while (!Peripheral::SR::TXE::read()) {
    }

    Peripheral::DR::write(data);

    while (!Peripheral::SR::RXNE::read()) {
    }

    const auto rx = Peripheral::DR::read();

    while (Peripheral::SR::BSY::read()) {
    }

    return rx;
  }
};
}