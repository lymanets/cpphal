#pragma once

#include <cstring>

#include "hal/core/configurator.hpp"
#include "hal/core/irq.hpp"

#include "hal/uart/options.hpp"
#include "hal/uart/events.hpp"
#include "advanced_config.hpp"

#include "impl/stm32_f1.hpp"

namespace hal::uart {
template <
  int PeripheralNumber,
  class BasicConfig,
  class AdvancedConfig = Advanced<>>
struct Driver
    : core::ConfiguratorSingleBase<typename traits<tag<PeripheralNumber>>::peripheral>,
      impl::Driver<mcu::policy::value, typename traits<tag<PeripheralNumber>>::peripheral, BasicConfig,
                   AdvancedConfig> {
private:
  using Peripheral = traits<tag<PeripheralNumber>>::peripheral;

  template <class... Events>
  struct EventHandler {
    static void handle() {
      (handle_event<Events>(), ...);
    }

  private:
    template <class Event>
    static void handle_event() {
      if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::ReadSR_DR_NoData>) {
        if (Event::event::pending()) {
          Event::handler::template run<Driver>();
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::ReadSR_DR>) {
        if (Event::event::pending()) {
          Event::handler::template run<Driver>(Event::event::read());
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::WriteTC0>) {
        if (Event::event::pending()) {
          Event::event::clear_irq_write_to_status_0();
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::None>) {
        if (Event::event::pending()) {
          Event::handler::template run<Driver>();
        }
      }
    }
  };

  using configurator = impl::Configurator<mcu::policy::value, Peripheral, BasicConfig, AdvancedConfig>;

  using SignalsMap = meta::mp_list<
    meta::mp_list<void, meta::mp_list<>>,
    meta::mp_list<options::direction::Tx, meta::mp_list<core::SignalType<core::SignalKind::Tx>>>,
    meta::mp_list<options::direction::Rx, meta::mp_list<core::SignalType<core::SignalKind::Rx>>>,
    meta::mp_list<options::direction::TxRx, meta::mp_list<
                    core::SignalType<core::SignalKind::Tx>, core::SignalType<core::SignalKind::Rx>>>,
    meta::mp_list<options::flow_control::None, meta::mp_list<>>,
    meta::mp_list<options::flow_control::Rts, meta::mp_list<core::SignalType<core::SignalKind::Rts>>>,
    meta::mp_list<options::flow_control::Cts, meta::mp_list<core::SignalType<core::SignalKind::Cts>>>,
    meta::mp_list<options::flow_control::RtsCts, meta::mp_list<
                    core::SignalType<core::SignalKind::Rts>, core::SignalType<core::SignalKind::Cts>>>
  >;

  using signals_type_pair = meta::mp_append<
    meta::mp_second<meta::mp_map_find<SignalsMap, typename configurator::basic::direction>>,
    meta::mp_second<meta::mp_map_find<SignalsMap, typename configurator::advanced::flowcontrol>>>;
  static_assert(!std::is_same_v<signals_type_pair, void>, "Unsupported Signal");

  static void irq_handler() {
    using handler = meta::mp_apply<EventHandler, typename configurator::advanced::events::value>;
    handler::handle();
  }

public:
  using irq_binding = irq::Binding<Peripheral::irq_number,
                                   irq_handler,
                                   !meta::mp_empty<typename configurator::advanced::events::value>::value>;

  using signals = core::resolve_signals_t<Peripheral, signals_type_pair>;

  template <class ClockConfig>
  static void apply() {
    configurator::template apply<ClockConfig>();
    if constexpr (irq_binding::enabled) {
      irq_binding::enable_irq();
    }
  }
};
}