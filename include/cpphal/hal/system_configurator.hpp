#pragma once
#include "configurator.hpp"
#include "hal/rcc/config.hpp"

namespace hal {
/*
Configures global resources MCU:
 -- RCC
 -- Flash latency
 -- PLL
 -- Clock tree
 -- Bus prescalers
 -- SysTick
 -- NVIC (optional)
*/

template <typename T>
concept IsSystemConfigurator = requires { typename T::system_tag; };

struct SystemTag {
};

template <class... Configs>
struct SystemConfigurator {
  using system_tag   = SystemTag;
  using Policy       = mcu::policy::value;
  using OptionHolder = core::OptionHolder<Configs...>;

  static void apply() {
    using Tree = rcc::Solver<Policy, Configs...>;
    rcc::Configurator<Policy, Tree>::apply();
  }
};
}