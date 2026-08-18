#pragma once

#include "base.hpp"

namespace hal::timer::impl {
template <class Peripheral, class Config>
struct Basic<mcu::policy::STM32F1Policy, Peripheral, Config> {
  using basic = BasicConfig<Peripheral, Config>;

  template <class ClockConfig>
  static void apply() {
  }
};
}