#pragma once
#include "field.hpp"

namespace mmio {
template <class Reg, unsigned Offset, unsigned Width, Access Mode, class Enum>
struct EnumField : Field<Reg, Offset, Width, Mode, Enum> {
  static Enum get() {
    return static_cast<Enum>(Field<Reg, Offset, Width, Mode, Enum>::read());
  }

  static void set(Enum e) {
    Field<Reg, Offset, Width, Mode, Enum>::write(static_cast<std::uint32_t>(e));
  }
};
} // namespace mmio