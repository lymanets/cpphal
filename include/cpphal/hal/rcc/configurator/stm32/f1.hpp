#pragma once

#include "hal/rcc/configurator/configurator_base.hpp"

#include "rcc.hpp"

namespace hal::rcc {
template <class Tree>
struct Configurator<mcu::policy::STM32F1Policy, Tree> {
  static void apply() {
    using FLASH     = mcu::FLASH;
    using RCC       = mcu::RCC;
    using CFGRField = RCC::CFGR;

    RCC::CR::HSEON::set();
    RCC::CR::HSERDY::wait();

    FLASH::ACR::write(Tree::ACR);

    CFGRField::write(Tree::CFGR);

    RCC::CR::PLLON::set();
    RCC::CR::PLLRDY::wait();

    CFGRField::SW::write(Tree::sw_bits());
    CFGRField::SWS::wait();
  }
};
}