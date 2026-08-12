#pragma once

#include <cstdint>

#include "access.hpp"
#include "field.hpp"

namespace mmio {
struct RegTag {
};

template <class Peripheral, std::uint32_t Offset, typename T = std::uint32_t,
          Access Mode = Access::ReadWrite>
struct Register {
  using reg_tag         = RegTag;
  using peripheral_type = Peripheral;
  using value_type      = T;

  static constexpr std::uint32_t  offset  = Offset;
  static constexpr std::uintptr_t address = Peripheral::BASE + Offset;

  static volatile T& ref() { return *reinterpret_cast<volatile T*>(address); }

  static T read()
    requires(Mode != Access::WriteOnly) {
    return ref();
  }

  template <unsigned FieldOffset, unsigned FieldWidth>
    requires(Mode != Access::ReadOnly)
  static void write_field(value_type value) {
    constexpr value_type mask = ((value_type{1} << FieldWidth) - 1) << FieldOffset;
    modify(mask, value << FieldOffset);
  }

  static void write(T value)
    requires(Mode != Access::ReadOnly) {
    ref() = value;
  }

  static void modify(T clear_mask, T set_mask)
    requires(Mode == Access::ReadWrite) {
    auto r = read();
    r      = (r & ~clear_mask) | set_mask;
    write(r);
  }

  template <std::size_t Bit>
  static void set_bit()
    requires(Mode != Access::ReadOnly) {
    static_assert(Bit < sizeof(T) * 8);

    ref() |= (T{1} << Bit);
  }

  template <std::size_t Bit>
  static void clear_bit()
    requires(Mode != Access::ReadOnly) {
    static_assert(Bit < sizeof(T) * 8);

    ref() &= ~(T{1} << Bit);
  }

  template <std::size_t Bit>
  [[nodiscard]]
  static bool bit()
    requires(Mode != Access::WriteOnly) {
    static_assert(Bit < sizeof(T) * 8);

    return (ref() & (T{1} << Bit)) != 0;
  }

  template <std::size_t Bit>
  static void toggle_bit()
    requires(Mode != Access::ReadOnly) {
    static_assert(Bit < sizeof(T) * 8);

    ref() ^= (T{1} << Bit);
  }
};
} // namespace mmio