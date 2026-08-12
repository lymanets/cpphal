#pragma once

#include <type_traits>
#include <cstdint>

#include "../policy/clock_policy.hpp"
#include "../policy/irq_policy.hpp"
#include "../policy/pin_policy.hpp"
#include "../policy/reset_policy.hpp"

namespace hal::core {
template <class T>
concept HasGpioPolicy = requires { typename T::gpio_policy; };

template <class>
struct EmptyPolicy {
};

template <class T>
using pin_policy_t =
std::conditional_t<HasGpioPolicy<T>, policy::PinPolicy<T>, EmptyPolicy<T>>;

template <class Derived, std::uintptr_t Base>
struct Peripheral
    : policy::ClockPolicy<Derived>,
      policy::ResetPolicy<Derived>,
      policy::IrqPolicy<Derived>,
      pin_policy_t<Derived> {
  static constexpr std::uintptr_t BASE = Base;
};
} // namespace hal