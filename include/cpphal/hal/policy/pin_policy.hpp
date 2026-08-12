#pragma once
#include "hal/gpio/pin.hpp"

namespace hal::policy {
template <class Derived>
struct PinPolicy {
  // template <uint8_t N, class C>
  // using Pin = gpio::Pin<Derived, N, C>;
};
} // namespace hal::policy