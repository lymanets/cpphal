#pragma once

#include <cstdint>
#include "meta/meta.hpp"
#include "signal_kind.hpp"

namespace hal::core {
template <class Peripheral, class Tag>
struct resolve_signal {
  using pair = meta::mp_map_find<
    typename Peripheral::signal_map,
    Tag
  >;

  static_assert(
      !std::is_same_v<pair, void>,
      "Signal is not supported by peripheral"
      );

  using type = meta::mp_second<pair>;
};

template <class Peripheral, class Tag>
using resolve_signal_t = resolve_signal<Peripheral, Tag>::type;

template <class Peripheral>
struct resolve_signals {
  template <class S>
  using fn = resolve_signal_t<Peripheral, S>;
};

template <class Peripheral, class Signals>
struct resolve_signals_impl {
  using type = meta::mp_transform_q<
    resolve_signals<Peripheral>,
  Signals
  >;
};

template <class Peripheral>
struct resolve_signals_impl<Peripheral, void> {
  using type = meta::mp_list<>;
};

template <class Peripheral, class Signals>
using resolve_signals_t = resolve_signals_impl<Peripheral, Signals>::type;

template <class List>
struct Signals {
  using value = List;
};

struct SignalTag {
};

template <class Peripheral, SignalKind Kind, class Pin>
struct Signal {
  using signal_tag = SignalTag;
  using peripheral = Peripheral;
  using pin = Pin;

  static constexpr SignalKind kind = Kind;
};
} // namespace hal