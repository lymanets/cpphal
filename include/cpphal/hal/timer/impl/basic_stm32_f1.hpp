#pragma once

#include "base.hpp"

namespace hal::timer::impl {
using namespace literals;

template <class Peripheral, class Config>
struct Basic<mcu::policy::STM32F1Policy, Peripheral, Config> {
private:
  using basic = BasicConfig<Peripheral, Config>;

  template <std::uint32_t Clock, std::uint32_t Frequency, bool AutoPeriod>
  struct PrescalerARR {
    static_assert(Clock % Frequency == 0 || AutoPeriod,
                  "timer::Basic: Frequency<> must divide the timer clock exactly; "
                  "enable AutoPeriod to automatically calculate PSC and ARR");
    static constexpr std::uint32_t Divisor = Clock / Frequency;

    static constexpr auto solve() {
      for (std::uint32_t psc = 0; psc <= 0xFFFF; ++psc) {
        const auto divider = psc + 1;

        if (Divisor % divider != 0) continue;

        const auto arr = Divisor / divider - 1;

        if (arr <= 0xFFFF) {
          return std::pair{psc, arr};
        }
      }
      static_assert(Clock % Frequency == 0 || AutoPeriod, "Can not find prescaler and period");
      return std::pair<uint32_t, uint32_t>{0xFFFFFFFF, 0xFFFFFFFF};
    }

    static constexpr auto value = solve();

    static constexpr std::uint32_t prescaler = value.first;
    static constexpr std::uint32_t period    = value.second;
  };

public:
  template <class ClockConfig>
  static void apply() {
    using clock_sys                = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    using clock_bus                = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    constexpr auto m               = clock_bus::frequency == clock_sys::frequency ? 1 : 2;
    constexpr auto timer_frequency = clock_bus::frequency * m;
    constexpr auto auto_period     = !std::is_same_v<typename basic::auto_period, void>;
    Peripheral::CR1::CEN::reset();
    using psc_arr = PrescalerARR<timer_frequency,
                                 basic::frequency::value,
                                 auto_period>;

    if constexpr (auto_period) {
      constexpr auto timer_freq = timer_frequency / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));
      static_assert(timer_freq == basic::frequency::value,
                    "Can not find prescaler and period");

      Peripheral::PSC::write(psc_arr::prescaler);
      Peripheral::ARR::write(psc_arr::period);
    } else {
      Peripheral::PSC::write(psc_arr::Divisor - 1);
      Peripheral::ARR::write(0xffff);
    }

    Peripheral::CNT::write(0);

    Peripheral::EGR::UG::set();

    Peripheral::SR::UIF::reset();
  }


  static void start() {
    Peripheral::CR1::CEN::set();
  }

  static void stop() {
    Peripheral::CR1::CEN::reset();
  }

  static void reset() {
    Peripheral::CNT::write(0);
  }

  static std::uint32_t counter() {
    return Peripheral::CNT::read();
  }

  static void delay_us(std::uint16_t us) {
    static_assert(
        basic::frequency::value == 1_MHz,
        "timer::Basic::delay_us requires 1 MHz frequency"
        );
    reset();
    const auto start = counter();

    while (static_cast<std::uint16_t>(counter() - start) < us) {
    }
  }
};
}