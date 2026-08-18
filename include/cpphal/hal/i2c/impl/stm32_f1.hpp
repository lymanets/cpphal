#pragma once

#include <bit>
#include <device.hpp>

#include "configurator.hpp"
#include "driver.hpp"
#include "hal/i2c/options.hpp"

namespace hal::i2c::impl {
using namespace literals;

template <auto freq>
struct SelectMultiplierTRISE {
  static_assert(freq == 100_kHz || freq == 400_kHz, "Invalid I2C frequency");
};

template <>
struct SelectMultiplierTRISE<100_kHz> {
  static constexpr auto value = 1;
};

template <>
struct SelectMultiplierTRISE<400_kHz> {
  static constexpr auto value = 0.3;
};

template <auto freq, bool duty>
struct SelectMultiplierCCR {
  static_assert(freq == 100_kHz || freq == 400_kHz, "Invalid I2C frequency");
};

template <bool duty>
struct SelectMultiplierCCR<100_kHz, duty> {
  static constexpr auto value = 2;
};

template <>
struct SelectMultiplierCCR<400_kHz, false> {
  static constexpr auto value = 3;
};

template <>
struct SelectMultiplierCCR<400_kHz, true> {
  static constexpr auto value = 25;
};

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

  template <auto psclk, auto freq, bool duty>
  struct ComputeCCR {
    static constexpr auto value = psclk / (SelectMultiplierCCR<freq, duty>::value * freq);
  };

  template <class ClockConfig>
  static consteval std::uint32_t trise_bits() {
    using clock_bus = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    return static_cast<uint32_t>((clock_bus::frequency / 1'000'000) * SelectMultiplierTRISE<
                                   basic::frequency::value>::value) + 1;
  }

  template <class ClockConfig>
  static consteval std::uint32_t ccr_bits() {
    using clock_bus     = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    constexpr auto duty = false;

    constexpr auto ccr = ComputeCCR<clock_bus::frequency, basic::frequency::value, duty>::value;
    return Peripheral::CCR::CCR_FIELD::encode(ccr);
  }

  static consteval std::uint32_t cr1_bits() {
    using cr1_events = encode_t<events_registers, typename Peripheral::CR1>;

    return Peripheral::CR1::PE::encode(1)
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

    constexpr auto ccr = ccr_bits<ClockConfig>();
    if constexpr (ccr != 0) Peripheral::CCR::write(ccr);

    constexpr auto trise = trise_bits<ClockConfig>();
    if constexpr (trise != 0) Peripheral::TRISE::write(trise);

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