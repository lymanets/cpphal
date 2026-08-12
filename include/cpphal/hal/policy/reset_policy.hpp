#pragma once
namespace hal::policy {
template <class Derived>
struct ResetPolicy {
  static void reset() {
    // Derived::reset_controller::pulse<Derived::reset_bit>();
  }
};
} // namespace hal::policy