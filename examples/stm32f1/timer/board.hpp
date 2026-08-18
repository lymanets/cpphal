#pragma once

#include <cpphal.hpp>

using namespace hal::literals;

using SystemTimer = hal::timer::System<hal::timer::Unit::msec>;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using GpioConfig = hal::gpio::Configurator<>;

using LogUart = hal::uart::Driver<
  2,
  hal::uart::config::Basic<
    hal::uart::options::BaudRate<115200>,
    hal::uart::options::DataBits<8>,
    hal::uart::options::StopBits<1>,
    hal::uart::options::parity::None,
    hal::uart::options::direction::TxRx
  >
>;

using BasicTimer = hal::timer::Basic<
  2,
  hal::timer::config::Basic<
    hal::timer::options::Frequency<1_MHz>
  >
>;

using BoardConfig = hal::Configurator<
  System,
  SystemTimer,
  GpioConfig,
  LogUart,
  BasicTimer
>;