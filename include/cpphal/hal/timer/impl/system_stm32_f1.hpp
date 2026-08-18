#pragma once

#include <device.hpp>

#include "configurator.hpp"
#include "hal/timer/unit.hpp"

namespace hal::timer::impl {
template <Unit U>
struct System<mcu::policy::STM32F1Policy, U> {
  template <class ClockConfig>
  static void apply() {
    using Sysclk = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    SysTick_Config(Sysclk::frequency / static_cast<int>(U));
  }
};
}