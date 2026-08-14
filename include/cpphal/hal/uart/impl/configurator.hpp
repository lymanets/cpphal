#pragma once

#include "hal/uart/tags.hpp"
#include "hal/core/event.hpp"

namespace hal::uart::impl {
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

  using baud      = typename Config::template get<tags::BaudRate>;
  using databits  = typename Config::template get<tags::DataBits>;
  using stopbits  = typename Config::template get<tags::StopBits>;
  using parity    = typename Config::template get<tags::Parity>;
  using direction = typename Config::template get<tags::Direction>;
};


template <class Peripheral, class Config>
struct Advanced {
  using peripheral = Peripheral;

  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;

  using flowcontrol = typename Config::template get<tags::FlowControl>;
};
}