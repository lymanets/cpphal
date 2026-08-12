#pragma once

#include <cstdint>

#include "meta/meta.hpp"

#include "signal_kind.hpp"
#include "on.hpp"

namespace hal::core {
struct EventTag {
};

namespace clear_policy {
struct None {
};

struct ReadSR_DR {
};

struct ReadSR_DR_NoData {
};

struct WriteTC0 {
};
}

template <class Peripheral, class Tag>
struct resolve_event {
  using pair = meta::mp_map_find<
    typename Peripheral::event_map,
    typename Tag::event
  >;

  static_assert(
      !std::is_same_v<pair, void>,
      "Event is not supported by peripheral"
      );

  using type = On<meta::mp_second<pair>, typename Tag::handler>;
};

template <class Peripheral, class Tag>
using resolve_event_t = resolve_event<Peripheral, Tag>::type;

template <class Peripheral>
struct resolve_events {
  template <class Event>
  using fn = resolve_event_t<Peripheral, Event>;
};

template <class Peripheral, class Events>
struct resolve_events_impl {
  using type = meta::mp_transform_q<
    resolve_events<Peripheral>,
    Events
  >;
};

template <class Peripheral>
struct resolve_events_impl<Peripheral, void> {
  using type = boost::mp11::mp_list<>;
};

template <class Peripheral, class Events>
using resolve_events_t = resolve_events_impl<Peripheral, Events>::type;

template <class Event>
struct get_enable {
  using type = typename Event::event::enable;
};

template <class Event>
using get_enable_t = typename get_enable<Event>::type;

template <class Events>
struct event_enable_bits_impl {
  using type = meta::mp_transform<
    get_enable_t,
    Events
  >;
};

template <>
struct event_enable_bits_impl<meta::list<>> {
  using type = meta::EmptyList;
};

template <>
struct event_enable_bits_impl<void> {
  using type = meta::EmptyList;
};


template <class Events>
using event_enable_bits_t = event_enable_bits_impl<Events>::type;

template <class ClearPolicy, class EnableBit, class DataBit, class... StatusBit>
struct Event {
  using policy = ClearPolicy;
  using enable = EnableBit;

  static bool pending() {
    bool result = (StatusBit::read() | ...) > 0;
    if constexpr (std::is_same_v<policy, clear_policy::ReadSR_DR_NoData>) {
      read();
    }
    return result;
  }

  static std::uint8_t read() {
    return DataBit::read();
  }

  static void enable_irq() {
    EnableBit::set();
  }

  static void disable_irq() {
    EnableBit::reset();
  }

  static void clear_irq_write_to_status_0() {
    (StatusBit::write(0) | ...);
  }
};
} // namespace hal