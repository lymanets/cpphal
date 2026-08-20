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


  template <class ClockConfig>
  static void apply() {
    using clock_sys = typename ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    using clock_bus = typename ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;

    constexpr auto timer_frequency =
        clock_bus::frequency == clock_sys::frequency
          ? clock_bus::frequency
          : clock_bus::frequency * 2;

    static_assert(
        basic::frequency::value > 0,
        "timer::Basic: invalid frequency"
        );
    static_assert(
        basic::frequency::value <= timer_frequency,
        "timer::Basic: frequency cannot be greater than timer clock"
        );

    impl::Basic<mcu::policy::value, typename traits<tag<Instance>>::peripheral, Config>::template apply<ClockConfig>();
  }
};
}