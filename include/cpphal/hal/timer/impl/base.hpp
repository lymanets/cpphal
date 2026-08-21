#pragma once

#include "hal/timer/unit.hpp"
#include "hal/timer/options.hpp"
#include "hal/timer/channel_config.hpp"

namespace hal::timer::impl {
template <class Policy, Unit U>
struct System {
};

template <class Policy, class Peripheral, class Config>
struct Basic {
};

template <class Policy, int Instance, class Config>
struct Pwm {
};

template <class Policy, int Instance, class Config>
struct OutputCompare {
};

template <class Peripheral, class Config>
struct BasicConfig {
  using peripheral = Peripheral;

  using frequency   = Config::template get<tags::Frequency>;
  using auto_period = Config::template get<tags::AutoPeriod>;


  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

template <class Peripheral, class Config>
struct PwmConfig {
  using peripheral = Peripheral;

  using frequency = Config::template get<tags::Frequency>;
  using channels  = Config::template get_list<tags::ChannelConfig>;

  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

template <class Peripheral, class Config>
struct OutputCompareConfig {
  using peripheral = Peripheral;

  using frequency = Config::template get<tags::Frequency>;
  using channels  = Config::template get_list<tags::ChannelConfig>;

  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

namespace detail {
template <class Channel>
struct has_channel {
  template <class T>
  using fn = std::is_same<typename T::template get<tags::Channel>, Channel>;
};

template <class List, int Channel>
struct has_channel_t {
  using index                 = meta::mp_find_if_q<List, has_channel<options::Channel<Channel>>>;
  static constexpr bool value = index::value != meta::mp_size<List>::value;
};

template <class List, int Channel>
struct get_channel_t {
  using index = meta::mp_find_if_q<List, has_channel<options::Channel<Channel>>>;
  using type  = meta::mp_at<List, index>;
};

template <class CCxE, class CCxP, class OCxM, class CCRx,
          class Channel,
          class ModeTag,
          template <class> class ModeImpl,
          template <class> class CompareFn>
static constexpr void configure_channel() {
  using mode     = ModeImpl<typename Channel::template get<ModeTag>>;
  using polarity = typename Channel::template get<tags::polarity>;
  using compare  = typename Channel::template get<tags::Initial>;
  static_assert(
      mode::valid,
      "timer: invalid channel mode");

  CCxE::reset();
  OCxM::write(mode::value);
  if constexpr (std::is_same_v<polarity, options::polarity::ActiveLow>) {
    CCxP::set();
  } else {
    CCxP::reset();
  }
  CCRx::write(CompareFn<compare>::value);
}

template <class ClockConfig, class ClockTag, std::uint32_t Frequency, class CounterType = std::uint16_t>
struct PrescalerARR {
private:
  using clock_sys         = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
  using clock_bus         = ClockConfig::OptionHolder::template get<ClockTag>;
  static constexpr auto m = clock_bus::frequency == clock_sys::frequency ? 1 : 2;

public:
  static constexpr auto          Clock   = clock_bus::frequency * m;
  static constexpr std::uint32_t Divisor = Clock / Frequency;

private:
  static constexpr CounterType MaxARR = std::is_same_v<CounterType, std::uint32_t> ? 0xFFFFFFFF : 0xFFFF;

  struct Result {
    bool        valid;
    CounterType prescaler;
    CounterType period;
  };

  static constexpr Result solve() {
    for (std::uint32_t psc = 0; psc <= MaxARR; ++psc) {
      const auto divider = psc + 1;
      if (Clock / divider < Frequency) break;
      if (Clock % divider != 0) continue;
      const auto arr = Divisor / divider - 1;
      if (arr <= MaxARR) return Result{true, static_cast<CounterType>(psc), static_cast<CounterType>(arr)};
    }

    return Result{false, 0, 0};
  }

  static constexpr auto value = solve();

public:
  static constexpr bool valid     = value.valid;
  static constexpr auto prescaler = value.prescaler;
  static constexpr auto period    = value.period;
};

template <class Peripheral, class instance_t, class channels, int channel, class T = uint16_t>
static void set_compare(T compare) {
  static_assert(channel >= 1 && channel <= instance_t::channels,
                "timer: channel should be in range");
  static_assert(has_channel_t<channels, channel>::value,
                "timer: channel is not configured");

  if constexpr (channel == 1) {
    Peripheral::CCR1::write(compare);
  } else if constexpr (channel == 2) {
    Peripheral::CCR2::write(compare);
  } else if constexpr (channel == 3) {
    Peripheral::CCR3::write(compare);
  } else if constexpr (channel == 4) {
    Peripheral::CCR4::write(compare);
  }
}

template <class Peripheral, class instance_t, class channels, int channel>
static void start_channel() {
  static_assert(channel >= 1 && channel <= instance_t::channels,
                "timer::OutputCompare: channel should be in range");
  static_assert(detail::has_channel_t<channels, channel>::value,
                "timer::OutputCompare: channel is not configured");

  if constexpr (channel == 1) {
    Peripheral::CCER::CC1E::set();
  } else if constexpr (channel == 2) {
    Peripheral::CCER::CC2E::set();
  } else if constexpr (channel == 3) {
    Peripheral::CCER::CC3E::set();
  } else if constexpr (channel == 4) {
    Peripheral::CCER::CC4E::set();
  }
}

template <class Peripheral, class instance_t, class channels, int channel>
static void stop_channel() {
  static_assert(channel >= 1 && channel <= instance_t::channels,
                "timer::OutputCompare: channel should be in range");
  static_assert(detail::has_channel_t<channels, channel>::value,
                "timer::OutputCompare: channel is not configured");

  if constexpr (channel == 1) {
    Peripheral::CCER::CC1E::reset();
  } else if constexpr (channel == 2) {
    Peripheral::CCER::CC2E::reset();
  } else if constexpr (channel == 3) {
    Peripheral::CCER::CC3E::reset();
  } else if constexpr (channel == 4) {
    Peripheral::CCER::CC4E::reset();
  }
}

template <class Peripheral>
static void start() {
  Peripheral::CR1::CEN::set();
}

template <class Peripheral>
static void stop() {
  Peripheral::CR1::CEN::reset();
}
}
}