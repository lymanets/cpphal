#pragma once

#include "hal/core/configurator.hpp"
#include "hal/core/irq.hpp"
#include "hal/core/traits.hpp"

#include "hal/timer/pwm_config.hpp"

#include "impl/pwm_stm32_f1.hpp"

namespace hal::timer {
template <int Instance, class Config>
struct Pwm
    : core::ConfiguratorSingleBase<typename traits<tag<Instance>>::peripheral>,
      impl::Pwm<mcu::policy::value, Instance, Config> {
private:
  using Peripheral = traits<tag<Instance>>::peripheral;

  using basic = impl::PwmConfig<Peripheral, Config>;

  static_assert(
      basic::initial_duty::value <= 100,
      "timer::Pwm: initial duty must be in range [0, 100]"
      );

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

  using SignalsMap = meta::mp_list<
    meta::mp_list<options::Channel<1>, meta::mp_list<core::SignalType<core::SignalKind::Ch1>>>,
    meta::mp_list<options::Channel<2>, meta::mp_list<core::SignalType<core::SignalKind::Ch2>>>,
    meta::mp_list<options::Channel<3>, meta::mp_list<core::SignalType<core::SignalKind::Ch3>>>,
    meta::mp_list<options::Channel<4>, meta::mp_list<core::SignalType<core::SignalKind::Ch4>>>
  >;

  template <class Entry>
  using channel_is_used = meta::mp_contains<typename basic::channels, meta::mp_first<Entry>>;

  template <class Entry>
  using get_signals = meta::mp_first<meta::mp_second<Entry>>;

  using signals_type_pair = meta::mp_append<
    meta::mp_transform<
      get_signals,
      meta::mp_filter<channel_is_used, SignalsMap>
    >
  >;

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
        "timer::Pwm: invalid frequency"
        );
    static_assert(
        basic::frequency::value <= timer_frequency,
        "timer::Pwm: frequency cannot be greater than timer clock"
        );

    impl::Pwm<mcu::policy::value, Instance, Config>::template apply<ClockConfig>();
  }
};
}