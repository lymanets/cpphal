#pragma once

#include <cpphal.hpp>

using namespace hal::literals;

struct RxNotEmptyHandler {
  template <class Driver>
  static void run(std::uint8_t byte) {
    Driver::write(byte);
  }
};

struct TxEmptyHandler {
  static void run() {
  }
};

using SystemTimer = hal::system_timer::Driver<hal::system_timer::Unit::msec>;

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
  >,
  hal::uart::Advanced<
    hal::uart::options::flow_control::None,
    hal::uart::options::Events<
      hal::core::On<hal::uart::events::RxNotEmpty, RxNotEmptyHandler>
    >
  >
>;

using BoardConfig = hal::Configurator<
  System,
  SystemTimer,
  GpioConfig,
  LogUart
>;