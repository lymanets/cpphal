#pragma once

#include <array>

#include <device.hpp>
#include "runtime/handlers.hpp"

namespace hal::irq {

template <class... Bindings>
struct VectorTableBuilder {
  using Vector = std::uint32_t;//void (*)();

  static constexpr std::size_t SystemVectors = 16;
  static constexpr std::size_t VectorCount   = SystemVectors + mcu::policy::value::irq_count;
  using VectorArray                          = std::array<Vector, VectorCount>;

  [[noreturn]]
  static void default_handler() {
    while (true) {
    }
  }

  static constexpr VectorArray build() {
    VectorArray table{};

    for (auto& entry : table) {
      entry = reinterpret_cast<Vector>(&default_handler);
    }

    // Initial MSP
    table[0] = static_cast<Vector>(mcu::policy::value::stack_top);

    // Cortex-M exceptions
    table[1] = reinterpret_cast<Vector>(&Reset_Handler);
    // table[2] = reinterpret_cast<Vector>(&default_handler);
    // table[3] = reinterpret_cast<Vector>(&default_handler);
    // table[4] = reinterpret_cast<Vector>(&default_handler);
    // table[5] = reinterpret_cast<Vector>(&default_handler); // BusFault_Handler
    // table[6] = reinterpret_cast<Vector>(&default_handler); // UsageFault_Handler
    //
    // table[11] = reinterpret_cast<Vector>(&default_handler); // SVC_Handler
    // table[12] = reinterpret_cast<Vector>(&default_handler); // DebugMon_Handler
    // table[14] = reinterpret_cast<Vector>(&default_handler); // PendSV_Handler
    // table[15] = reinterpret_cast<Vector>(&default_handler); // SysTick_Handler

    ((table[SystemVectors + Bindings::irq_number] = reinterpret_cast<Vector>(Bindings::handler)), ...);

    return table;
  }
};
}