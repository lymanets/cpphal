#pragma once

#include "hal/timer/unit.hpp"
#include "hal/timer/options.hpp"

namespace hal::timer::impl {
template <class Policy, Unit U>
struct System {
};

template <class Policy, class Peripheral, class Config>
struct Basic {
};

template <class Policy, class Peripheral, class Config>
struct Pwm {
};

template <class Peripheral, class Config>
struct BasicConfig {
  using peripheral = Peripheral;

  using frequency = Config::template get<tags::Frequency>;
  using auto_period = Config::template get<tags::AutoPeriod>;


  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

template <class Peripheral, class Config>
struct PwmConfig {
  using peripheral = Peripheral;

  using frequency = Config::template get<tags::Frequency>;
  using channels = Config::template get_list<tags::Channel>;
  using initial_duty = Config::template get<tags::InitialDuty>;
  using pwm_mode = Config::template get<tags::pwm_mode>;


  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

}