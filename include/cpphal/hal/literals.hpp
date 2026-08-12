#pragma once

#include <cstdint>

namespace hal::literals {
consteval std::uint32_t operator"" _Hz(unsigned long long value) {
  return static_cast<std::uint32_t>(value);
}

consteval std::uint32_t operator"" _kHz(unsigned long long value) {
  return static_cast<std::uint32_t>(value * 1'000);
}

consteval std::uint32_t operator"" _MHz(unsigned long long value) {
  return static_cast<std::uint32_t>(value * 1'000'000);
}
} // namespace hal::literals