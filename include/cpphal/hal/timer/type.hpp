#pragma once
#include <cstdint>

namespace hal::timer {
enum class Type : uint8_t {
  Basic,
  GeneralPurpose,
  Advanced
};
}