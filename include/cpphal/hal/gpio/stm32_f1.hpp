#pragma once

#include <concepts>

#include "types.hpp"
#include "pin.hpp"
#include "hal/core/traits.hpp"

namespace hal::gpio {
template <Mode C>
concept OutputConfig = C == Mode::Output;

template <class C>
concept InputConfig = C::mode == Mode::Input;

template <class Port, std::uint8_t Number, class C, bool Enable>
struct PinImpl<STM32F1Policy, Port, Number, C, Enable> {
private:
  static constexpr bool initialized = Enable;

  using config  = C;

public:
  using port    = typename traits<Port>::peripheral;
  using CR_REG = meta::mp_if_c<Number < 8, typename port::CRL, typename port::CRH>;


  // static constexpr std::uint8_t number = Number;
  static constexpr std::uint32_t offset = (Number % 8) * 4;

  static constexpr std::uint32_t pin = Number;

private:
  static void set_pull() {
    switch (config::pull) {
      case Pull::None:
        break;

      case Pull::Up:
        port::ODR::template set_bit<Number>();
        break;

      case Pull::Down:
        port::ODR::template clear_bit<Number>();
        break;
    }
  }

public:
  template <Mode M = config::mode>
    requires OutputConfig<M>
  static void set() {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    port::BSRR::template set_bit<Number>();
  }

  template <Mode M = config::mode>
    requires OutputConfig<M>
  static void reset() {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    port::BRR::template clear_bit<Number>();
  }

  template <Mode M = config::mode>
    requires OutputConfig<M>
  static void toggle() {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    port::BSRR::template toggle_bit<Number>();
  }

  static bool read() {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    return port::IDR::template bit<Number>();
  }

  static bool output() {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    return port::ODR::template bit<Number>();
  }

  static void write(bool value) {
    static_assert(initialized,
                  "Pin was not configured. Add it to hal::gpio::Configurator.");
    value ? set() : reset();
  }

  static consteval std::uint32_t cr_bits() {
    if constexpr (config::mode == Mode::Input) {
      if constexpr (config::pull == Pull::None) {
        // MODE=00 CNF=01
        return 0b0100;
      } else {
        // MODE=00 CNF=10
        return 0b1000;
      }
    } else if constexpr (config::mode == Mode::Output) {
      constexpr std::uint32_t mode_bits = config::speed == Speed::Low
                                            ? 0b10
                                            : config::speed == Speed::Medium
                                            ? 0b01
                                            : 0b11;

      constexpr std::uint32_t cnf = config::type == OutputType::PushPull ? 0b00 : 0b01;

      return (cnf << 2) | mode_bits;
    } else if constexpr (config::mode == Mode::Alternate) {
      constexpr std::uint32_t mode_bits = config::speed == Speed::Low
                                            ? 0b10
                                            : config::speed == Speed::Medium
                                            ? 0b01
                                            : 0b11;

      constexpr std::uint32_t cnf = config::type == OutputType::PushPull ? 0b10 : 0b11;

      return (cnf << 2) | mode_bits;
    } else if constexpr (config::mode == Mode::Analog) {
      // MODE=00 CNF=00
      return 0b0000;
    }
    assert("Unknown GPIO pin configuration");
    return 0;
  }

  static constexpr std::uint32_t CR = cr_bits() << offset;
};
}