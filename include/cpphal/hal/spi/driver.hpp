#pragma once

#include <cstring>

#include "hal/core/configurator.hpp"
#include "hal/core/irq.hpp"

#include "options.hpp"
#include "advanced_config.hpp"

#include "impl/stm32_f1.hpp"

namespace hal::spi {
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
          Event::handler::run();
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::ReadSR_DR>) {
        if (Event::event::pending()) {
          Event::handler::run(Event::event::read());
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::WriteTC0>) {
        if (Event::event::pending()) {
          Event::event::clear_irq_write_to_status_0();
        }
      } else if constexpr (std::is_same_v<typename Event::event::policy, core::clear_policy::None>) {
        if (Event::event::pending()) {
          Event::handler::run();
        }
      }
    }
  };

  using irq_binding  = irq::Binding<Peripheral::irq_number>;
  using configurator = impl::Configurator<mcu::policy::value, Peripheral, BasicConfig, AdvancedConfig>;

  static void irq_handler() {
    using handler = meta::mp_apply<EventHandler, typename configurator::advanced::events::value>;
    handler::handle();
  }

  using SignalsMap = meta::mp_list<
    meta::mp_list<options::direction::TransmitOnly, meta::mp_list<core::SignalType<core::SignalKind::Mosi>>>,
    meta::mp_list<options::direction::ReceiveOnly, meta::mp_list<core::SignalType<core::SignalKind::Miso>>>,
    meta::mp_list<options::direction::FullDuplex, meta::mp_list<
                    core::SignalType<core::SignalKind::Mosi>, core::SignalType<core::SignalKind::Miso>>>
  >;

  template <class M>
  using direction_resolver = option_resolver_t<
    M,
    typename configurator::basic::direction,
    options::direction::FullDuplex
  >;

  using signals_type_pair = meta::mp_append<
    meta::mp_second<direction_resolver<SignalsMap>>,
    meta::mp_list<core::SignalType<core::SignalKind::Sck>>>;
  static_assert(!std::is_same_v<signals_type_pair, void>, "Unsupported Signal");

public:
  using signals = core::resolve_signals_t<Peripheral, signals_type_pair>;

  template <class ClockConfig>
  static void apply() {
    configurator::template apply<ClockConfig>();
    if constexpr (!meta::mp_empty<typename configurator::advanced::events>::value) {
      irq_binding::apply(irq_handler);
    }
  }
};
}