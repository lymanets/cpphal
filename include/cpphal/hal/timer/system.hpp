#pragma once

#include <cstring>

#include "hal/core/configurator.hpp"
#include "hal/core/irq.hpp"

#include "impl/system_stm32_f1.hpp"
#include "hal/timer/unit.hpp"

namespace hal::timer {
template <Unit U>
struct System {
private:
  static constexpr Unit u_         = U;
  using counter_t                  = std::uint32_t;
  static inline counter_t counter_ = 0;
  counter_t               timeout_ = 0;
  bool                    stop_    = false;

  using configurator = impl::System<mcu::policy::value, U>;

  static void irq_handler() {
    counter_++;
  }

public:
  using signals = meta::mp_list<>;

  using irq_binding = irq::Binding<SysTick_IRQn, irq_handler, true>;

  template <class ClockConfig>
  static void apply() {
    configurator::template apply<ClockConfig>();
    if constexpr (irq_binding::enabled) {
      irq_binding::enable_irq();
    }
  }

  static counter_t ticks() { return counter_; }

  void mark(counter_t value) {
    timeout_ = counter_ + value;
    stop_    = false;
  }

  [[nodiscard]]
  bool isTimeout() const { return !stop_ && counter_ >= timeout_; }

  [[nodiscard]]
  counter_t elapsed() const { return timeout_ - counter_; }

  bool update(counter_t value) { return isTimeout() ? (mark(value), true) : false; }

  void stop() { stop_ = true; }


  template <Unit u = u_>
  static void delay_ms(counter_t msec)
    requires (u == Unit::msec) {
    const auto start = ticks();

    while (ticks() - start < msec) {
    }
  }
};
}