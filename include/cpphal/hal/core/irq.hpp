#pragma once

#include "device.hpp"

namespace hal::irq {
using IRQHandlerFn = void (*)();

template <IRQn_Type IRQNumber>
struct Binding {
  static void apply(IRQHandlerFn irq_handler) {
    NVIC_SetVector(
        IRQNumber,
        reinterpret_cast<uint32_t>(&irq_handler)
        );
    NVIC_EnableIRQ(IRQNumber);
  }

  static void enable_irq() {
    NVIC_EnableIRQ(IRQNumber);
  }

  static void disable_irq() {
    NVIC_DisableIRQ(IRQNumber);
  }
};
} // namespace hal::irq