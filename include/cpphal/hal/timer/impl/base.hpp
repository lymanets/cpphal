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

  using frequency    = Config::template get<tags::Frequency>;
  using channels     = Config::template get_list<tags::Channel>;
  using initial_duty = Config::template get<tags::InitialDuty>;
  using pwm_mode     = Config::template get<tags::pwm_mode>;
  using polarity     = Config::template get<tags::polarity>;


  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

template <class Peripheral, class Config>
struct OutputCompareConfig {
  using peripheral = Peripheral;

  using frequency       = Config::template get<tags::Frequency>;
  using channels        = Config::template get_list<tags::Channel>;
  using polarity        = Config::template get<tags::polarity>;
  using initial_compare = Config::template get<tags::InitialCompare>;
  using oc_mode         = Config::template get<tags::output_compare_mode>;


  using events  = core::resolve_events_t<Peripheral, typename Config::template get<tags::Events>>;
  using enables = core::event_enable_bits_t<events>;
};

namespace detail {
template <class CCxE, class CCxP, class OCxM, class CCRx,
          class Mode, class Polarity, auto Compare>
static constexpr void configure_channel() {
  CCxE::reset();
  OCxM::write(Mode::value);
  if constexpr (std::is_same_v<Polarity, options::polarity::ActiveLow>) {
    CCxP::set();
  } else {
    CCxP::reset();
  }
  CCRx::write(Compare);
}

template <class ClockConfig, class ClockTag, std::uint32_t Frequency, class CounterType = std::uint16_t>
struct PrescalerARR {
private:
  using clock_sys                       = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
  using clock_bus                       = ClockConfig::OptionHolder::template get<ClockTag>;
  static constexpr auto m               = clock_bus::frequency == clock_sys::frequency ? 1 : 2;

public:
  static constexpr auto Clock = clock_bus::frequency * m;
  static constexpr std::uint32_t Divisor = Clock / Frequency;

private:
  static constexpr CounterType MaxARR = std::is_same_v<CounterType, std::uint32_t> ? 0xFFFFFFFF : 0xFFFF;

  static constexpr auto solve() {
    for (std::uint32_t psc = 0; psc <= MaxARR; ++psc) {
      const auto divider = psc + 1;
      if (Clock / divider < Frequency) break;
      if (Clock % divider != 0) continue;
      const auto arr = Divisor / divider - 1;
      if (arr <= MaxARR) return std::pair{static_cast<CounterType>(psc), static_cast<CounterType>(arr)};
    }

    return std::pair<CounterType, CounterType>{0xFFFFFFFF, 0xFFFFFFFF};
  }

  static constexpr auto value = solve();

public:
  static constexpr bool valid     = value.first != 0xFFFFFFFF;
  static constexpr auto prescaler = value.first;
  static constexpr auto period    = value.second;
};
}
}