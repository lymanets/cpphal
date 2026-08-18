#pragma once

#include "hal/core/configurator.hpp"
#include "hal/core/irq.hpp"
#include "hal/core/traits.hpp"

#include "hal/timer/basic_config.hpp"

#include "impl/basic_stm32_f1.hpp"

namespace hal::timer {
template <int Instance, class Config>
struct Basic
    : core::ConfiguratorSingleBase<typename traits<tag<Instance>>::peripheral>,
      impl::Basic<mcu::policy::value, typename traits<tag<Instance>>::peripheral, Config> {
private:
  using Peripheral = traits<tag<Instance>>::peripheral;

  using basic = impl::BasicConfig<Peripheral, Config>;

  template <class... Events>
  struct EventHandler {
    static void handle() {
      (handle_event<Events>(), ...);
    }

  private:
    template <class Event>
    static void handle_event() {
    }
  };

  static void irq_handler() {
    using handler = meta::mp_apply<EventHandler, typename basic::events::value>;
    handler::handle();
  }

  using signals_type_pair = meta::mp_list<>;

public:
  using irq_binding = irq::Binding<Peripheral::irq_number,
                                   irq_handler,
                                   !meta::mp_empty<typename basic::events::value>::value>;
  using signals = core::resolve_signals_t<Peripheral, signals_type_pair>;


};
}