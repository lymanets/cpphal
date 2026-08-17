#pragma once

#include "hal/i2c/tags.hpp"
#include "hal/core/event.hpp"

namespace hal::i2c::impl {
template <
  class Policy,
  class Peripheral,
  class BasicConfig,
  class AdvancedConfig>
struct Configurator {
};

template <class Peripheral, class Config>
struct Basic {
  using peripheral = Peripheral;

  using frequency = Config::template get<tags::Frequency>;
};


template <class Peripheral, class Config>
struct Advanced {
  using peripheral = Peripheral;

  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
  using timeout = Config::template get<tags::Timeout>;
};
}