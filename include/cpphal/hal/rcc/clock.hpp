#pragma once

namespace hal::rcc {
template <class Register, uint8_t Bit>
struct Clock {
  using reg                    = Register;
  static constexpr uint8_t bit = Bit;

  static void enable() { Register::template set_bit<Bit>(); }
  static void disable() { Register::template clear_bit<Bit>(); }
  static bool enabled() { return Register::template bit<Bit>(); }
};

} // namespace hal::rcc