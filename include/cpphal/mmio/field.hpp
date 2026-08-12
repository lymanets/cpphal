#pragma once
#include <cstdint>

#include "access.hpp"

namespace mmio {
template <class Register, unsigned Offset, unsigned Width, Access Mode, typename Enum = void>
struct Field {
  using register_type = Register;
  using value_type    = typename Register::value_type;

  static constexpr value_type mask =
      ((value_type{1} << Width) - 1) << Offset;

  static constexpr value_type encode(value_type value) {
    return (value << Offset) & mask;
  }

  static constexpr value_type insert(value_type reg,
                                     value_type value) {
    return (reg & ~mask) | encode(value);
  }

  static constexpr value_type extract(value_type reg) {
    return (reg & mask) >> Offset;
  }

  static void write(value_type value)
    requires (Mode != Access::ReadOnly) {
    Register::modify(mask, encode(value));
  }

  static void set() {
    Register::modify(mask, 1);
  }

  static void reset() {
    Register::modify(mask, 0);
  }

  static void wait()
    requires (Mode == Access::ReadOnly) {
    while (read() != 0);
  }

  static value_type read()
    requires (Mode != Access::WriteOnly) {
    return extract(Register::read());
  }
};

// template <class Reg, unsigned NumBit>
// using Bit = Field<Reg, NumBit, 1>;
} // namespace mmio