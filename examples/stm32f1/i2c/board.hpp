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
  hal::uart::Basic<
    hal::uart::options::BaudRate<115200>,
    hal::uart::options::DataBits<8>,
    hal::uart::options::StopBits<1>,
    hal::uart::options::parity::None,
    hal::uart::options::direction::TxRx
  >
>;

using I2C = hal::i2c::Driver<
  1,
  SystemTimer,
  hal::i2c::Basic<
    hal::i2c::options::Frequency<100_kHz>
  >
>;

using BoardConfig = hal::Configurator<
  System,
  SystemTimer,
  GpioConfig,
  LogUart,
  I2C
>;