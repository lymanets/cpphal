#pragma once

#include "hal/spi/tags.hpp"
#include "hal/core/event.hpp"

namespace hal::spi::impl {
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

  using mode           = Config::template get<tags::Mode>;
  using clock_polarity = Config::template get<tags::ClockPolarity>;
  using clock_phase    = Config::template get<tags::ClockPhase>;
  using bit_order      = Config::template get<tags::BitOrder>;
  using data_bits      = Config::template get<tags::DataBits>;
  using baud           = Config::template get<tags::Baud>;
  using direction      = Config::template get<tags::Direction>;
};


template <class Peripheral, class Config>
struct Advanced {
  using peripheral = Peripheral;

  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};
}