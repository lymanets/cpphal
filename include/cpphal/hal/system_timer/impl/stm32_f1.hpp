#pragma once

#include <device.hpp>

#include "configurator.hpp"
#include "hal/system_timer/options.hpp"

namespace hal::system_timer::impl {
template <Unit U>
struct Configurator<mcu::policy::STM32F1Policy, U> {
  template <class ClockConfig>
  static void apply() {
    using Sysclk = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    SysTick_Config(Sysclk::frequency / static_cast<int>(U));
  }
};
}