#pragma once
#include <cstdint>

namespace hal::gpio {
enum class Mode { Input, Output, Alternate, Analog };

enum class Pull { None, Up, Down };

enum class Speed { Low, Medium, High, VeryHigh };

enum class OutputType { PushPull, OpenDrain };

struct NoAlternate {
};

template <std::uint8_t N>
struct Alternate {
  static constexpr std::uint8_t value = N;
};
}