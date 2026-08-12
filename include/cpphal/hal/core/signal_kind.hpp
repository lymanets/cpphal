#pragma once

namespace hal::core {
enum class SignalKind {
  // UART
  Tx,
  Rx,
  Cts,
  Rts,
  Ck,
  // SPI
  Sck,
  Miso,
  Mosi,
  Nss,
  // I2C
  Scl,
  Sda,
  Smba,
  Smbal,
  Ch1,
  Ch2,
  Ch3,
  Ch4,
  Ch1n,
  Ch2n,
  Ch3n,
  Bkin,
  Etr,
};

template <SignalKind Signal>
struct SignalType {
  static constexpr SignalKind value = Signal;
};
} // namespace hal