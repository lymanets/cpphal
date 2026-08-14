#pragma once

#include <cpphal.hpp>

using namespace hal::literals;

struct RxNotEmptyHandler {
  static void run(std::uint8_t byte) {
  }
};

struct TxEmptyHandler {
  static void run() {
  }
};

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using GpioConfig = hal::gpio::Configurator<
  PinDef(A, 0, OutputPushPull),
  PinDef(A, 1, Input),
  PinDef(A, 2, Input),
  PinDef(B, 1, Input),
  PinDef(C, 2, Input),
  PinDef(B, 2, Input)
>;

using LogUart = hal::uart::Driver<
  1,
  hal::uart::Basic<
    hal::uart::options::Baud<115200>,
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
  GpioConfig,
  LogUart
>;