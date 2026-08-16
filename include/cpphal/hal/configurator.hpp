#pragma once

#include "hal/system_configurator.hpp"
#include "hal/vector_table_builder.hpp"

#include "hal/gpio/gpio.hpp"
#include "hal/uart/uart.hpp"
#include "hal/spi/spi.hpp"
#include "hal/system_timer/system_timer.hpp"

namespace hal {
template <class System, class SystemTimer, class GpioConfig, class... Configs>
  requires IsSystemConfigurator<System> && core::IsAllConfigurators<Configs...>
struct Configurator {
  using VectorTable = irq::VectorTableBuilder<typename Configs::irq_binding...>;

  static constexpr void apply() {
    using Signals   = meta::extract<get_signal_pin, meta::mp_append<typename Configs::signals...>>;
    using AllGroups = meta::mp_append<
      typename GpioConfig::Group,
      meta::group_by<gpio::internal::get_port, gpio::internal::get_pin, Signals>,
      typename Configs::Group...
    >;

    using Clocks = meta::extract<get_key, AllGroups>;
    System::apply();
    EnableClocks<meta::extract<get_clock, Clocks>>::apply();
    SystemTimer::template apply<System>();
    GpioConfig::template apply<Signals>();
    (Configs::template apply<System>(), ...);
  }

private:
  struct get_key {
    template <class T>
    using fn = typename T::key;
  };

  struct get_port {
    template <class T>
    using fn = typename T::port;
  };

  struct get_signal_pin {
    template <class T>
    using fn = typename T::pin;
  };

  struct get_clock {
    template <class T>
    using fn = typename T::clock;
  };

  struct get_register {
    template <class T>
    using fn = T::reg;
  };

  struct get_bit {
    template <class T>
    using fn = T;
  };

  template <class T>
  struct clock_mask_traits {
    using value_type                  = typename T::reg::value_type;
    static constexpr value_type value = 1 << T::bit;
  };

  template <class Clocks>
  struct EnableClocks {
    static void apply() {
      using ListOfClocks        = meta::mp_unique<Clocks>;
      using ClockRegs           = meta::group_by<get_register, get_bit, ListOfClocks>;
      using GroupedRegsWithMask = meta::mp_transform_q<mask::pair_to_update_q<clock_mask_traits>, ClockRegs>;
      meta::mp_for_each<GroupedRegsWithMask>([](auto x) {
        using Update = decltype(x);
        Update::apply();
      });
    }
  };
};
}