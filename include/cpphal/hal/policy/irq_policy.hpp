#pragma once
namespace hal::policy {
template <class Derived>
struct IrqPolicy {
  static constexpr int irq = Derived::irq_number;

  static void enable_irq() { Derived::nvic::enable(irq); }

  static void disable_irq() { Derived::nvic::disable(irq); }
};
} // namespace hal::policy