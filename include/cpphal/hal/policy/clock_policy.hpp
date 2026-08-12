#pragma once

namespace hal::policy {
template <class Derived>
struct ClockPolicy {
  static void enable_clock() { Derived::clock::enable(); }
  static void disable_clock() { Derived::clock::disable(); }
  static bool clock_enabled() { return Derived::clock::enabled(); }
};
} // namespace hal::policy