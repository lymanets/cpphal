#pragma once

#include <bit>
#include <device.hpp>

#include "configurator.hpp"
#include "driver.hpp"
#include "hal/i2c/options.hpp"

namespace hal::i2c::impl {
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

  template <class ClockConfig>
  static consteval std::uint32_t ccr_bits() {
    using clock_bus = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;

    constexpr auto ccr = clock_bus::frequency / (2 * basic::frequency::value);
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
    constexpr auto cr1 = cr1_bits();
    if constexpr (cr1 != 0) Peripheral::CR1::write(cr1);

    constexpr auto cr2 = cr2_bits();
    if constexpr (cr2 != 0) Peripheral::CR2::write(cr2);

    constexpr auto ccr = ccr_bits<ClockConfig>();
    if constexpr (ccr != 0) Peripheral::CCR::write(ccr);
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