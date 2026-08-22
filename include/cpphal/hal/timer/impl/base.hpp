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
          class CompareFn>
static constexpr void configure_channel() {
  using mode      = typename Channel::template get<ModeTag>;
  using mode_impl = ModeImpl<mode>;
  using polarity  = typename Channel::template get<tags::polarity>;
  using compare   = typename Channel::template get<tags::Initial>;
  static_assert(
      mode_impl::valid,
      "timer: invalid channel mode");

  CCxE::reset();
  OCxM::write(mode_impl::value);
  if constexpr (std::is_same_v<polarity, options::polarity::ActiveLow>) {
    CCxP::set();
  } else {
    CCxP::reset();
  }
  CCRx::write(CompareFn::template fn<compare, mode>::value);
}

template <class T>
struct OutputCompareModeImpl {
  static constexpr bool    valid = false;
  static constexpr uint8_t value = 0;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::Frozen> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b000;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::Active> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b001;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::Inactive> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b010;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::Toggle> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b011;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::PwmActiveHigh> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b110;
};

template <>
struct OutputCompareModeImpl<options::output_compare_mode::PwmActiveLow> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b111;
};

template <class CCER, bool polarity, int channel>
struct make_ccer_t {
  static constexpr uint32_t value = 0;
};

template <class CCER, bool polarity>
struct make_ccer_t<CCER, polarity, 1> {
  static constexpr uint32_t value = CCER::CC1E::encode(0) | CCER::CC1P::encode(polarity);
};

template <class CCER, bool polarity>
struct make_ccer_t<CCER, polarity, 2> {
  static constexpr uint32_t value = CCER::CC2E::encode(0) | CCER::CC2P::encode(polarity);
};

template <class CCER, bool polarity>
struct make_ccer_t<CCER, polarity, 3> {
  static constexpr uint32_t value = CCER::CC3E::encode(0) | CCER::CC3P::encode(polarity);
};

template <class CCER, bool polarity>
struct make_ccer_t<CCER, polarity, 4> {
  static constexpr uint32_t value = CCER::CC4E::encode(0) | CCER::CC4P::encode(polarity);
};

template <class CCER>
struct add_ccer_t {
  template <class State, class T>
  using fn = std::integral_constant<uint32_t,
                                    State::value | make_ccer_t<
                                      CCER,
                                      std::is_same_v<
                                        typename T::template get<tags::polarity>,
                                        options::polarity::ActiveLow>,
                                      T::template get<tags::Channel>::value
                                    >::value>;
};

template <class CCMR, uint32_t mode, int channel>
struct make_ccmr_t {
  static constexpr uint32_t value = 0;
};

template <class CCMR, uint32_t mode>
struct make_ccmr_t<CCMR, mode, 1> {
  static constexpr uint32_t value = CCMR::OC1M::encode(mode);
};

template <class CCMR, uint32_t mode>
struct make_ccmr_t<CCMR, mode, 2> {
  static constexpr uint32_t value = CCMR::OC2M::encode(mode);
};

template <class CCMR, uint32_t mode>
struct make_ccmr_t<CCMR, mode, 3> {
  static constexpr uint32_t value = CCMR::OC3M::encode(mode);
};

template <class CCMR, uint32_t mode>
struct make_ccmr_t<CCMR, mode, 4> {
  static constexpr uint32_t value = CCMR::OC4M::encode(mode);
};

template <class T>
struct validate_ccmr_fn {
  using mode = OutputCompareModeImpl<typename T::template get<tags::output_compare_mode>>;
  static_assert(
      mode::valid,
      "timer: invalid channel mode");

  static constexpr uint32_t value = mode::value;
};

template <class CCMR>
struct add_ccmr_t {
  template <class State, class T>
  using fn = std::integral_constant<uint32_t,
                                    State::value | make_ccmr_t<
                                      CCMR,
                                      validate_ccmr_fn<T>::value,
                                      T::template get<tags::Channel>::value
                                    >::value>;
};

template <class channels, class CCER>
static consteval uint32_t make_ccer() {
  return meta::mp_fold_q<channels,
                         std::integral_constant<uint32_t, 0>,
                         add_ccer_t<CCER>>::value;
}

template <class List>
struct ChannelFn {
  template <class Entry>
  using fn = meta::mp_contains<List,
                               typename Entry::template get<tags::Channel>>;
};

template <class channels, class List, class CCMR>
static consteval uint32_t make_ccmr() {
  using valid_channels = meta::mp_filter_q<ChannelFn<List>, channels>;
  using result         = meta::mp_fold_q<valid_channels,
                                 std::integral_constant<uint32_t, 0>,
                                 add_ccmr_t<CCMR>>;
  return result::value;
}

template <class channels, class P, int channel>
static consteval uint32_t make_ccr() {
  using channel_t = typename get_channel_t<channels, channel>::type;
  using mode      = OutputCompareModeImpl<typename channel_t::template get<tags::output_compare_mode>>;
  using init      = typename channel_t::template get<tags::Initial>;
  static_assert(
      mode::valid,
      "timer: invalid channel mode");
  return std::is_same_v<mode, options::output_compare_mode::PwmActiveHigh> ||
         std::is_same_v<mode, options::output_compare_mode::PwmActiveLow>
           ? (P::period * init::value) / 100
           : init::value;
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
  static_assert(has_channel_t<channels, channel>::value,
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
  static_assert(has_channel_t<channels, channel>::value,
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