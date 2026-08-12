#pragma once


namespace hal::core {
template <class Event, class Handler>
struct On {
  using event   = Event;
  using handler = Handler;
};
}