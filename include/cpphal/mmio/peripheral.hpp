#pragma once
#include <cstdint>

namespace mmio {
template <std::uintptr_t BaseAddress>
struct Peripheral {
  static constexpr std::uintptr_t BASE = BaseAddress;
};
} // namespace mmio