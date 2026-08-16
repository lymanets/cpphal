#pragma once

#include "device.hpp"

namespace hal::irq {
using IRQHandlerFn = void (*)();

template <IRQn_Type IRQNumber, IRQHandlerFn Handler, bool Enabled>
struct Binding {
  static constexpr IRQn_Type    irq_number = IRQNumber;
  static constexpr IRQHandlerFn handler    = Handler;
  static constexpr bool         enabled    = Enabled;

  static void enable_irq() {
    if constexpr (IRQNumber >= 0) {
      NVIC_EnableIRQ(IRQNumber);
    }
  }

  static void disable_irq() {
    if constexpr (IRQNumber >= 0) {
      NVIC_DisableIRQ(IRQNumber);
    }
  }
};
} // namespace hal::irq