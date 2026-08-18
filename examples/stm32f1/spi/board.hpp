#pragma once

#include <cpphal.hpp>

#include "pn532.hpp"

using namespace hal::literals;

using SystemTimer = hal::timer::System<hal::timer::Unit::msec>;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using GpioConfig = hal::gpio::Configurator<PinDef(A, 4, OutputPushPull)>;

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

using NFC_SPI = hal::spi::Driver<
  1,
  hal::spi::Basic<
    hal::spi::options::mode::Master,
    hal::spi::options::clock_phase::FirstEdge,
    hal::spi::options::clock_polarity::Low,
    hal::spi::options::bit_order::LSBFirst,
    hal::spi::options::Baud<2_MHz>,
    hal::spi::options::DataBits<8>
  >,
  hal::spi::Advanced<
    hal::spi::options::Events<
      // hal::core::On<hal::spi::events::TxEmpty, RxNotEmptyHandler>
    >
  >
>;

using pn532_t = PN532<NFC_SPI, GpioConfig::GetPin<hal::gpio::port::A, 4>, SystemTimer, LogUart>;

using BoardConfig = hal::Configurator<
  System,
  SystemTimer,
  GpioConfig,
  LogUart,
  NFC_SPI
>;