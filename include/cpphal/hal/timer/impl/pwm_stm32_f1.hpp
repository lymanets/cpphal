#pragma once

#include "base.hpp"

namespace hal::timer::impl {
using namespace literals;

template <class T>
struct PwmModeImpl {
  static constexpr bool    valid = false;
  static constexpr uint8_t value = 0;
};

template <>
struct PwmModeImpl<void> {
  // Default value
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b110;
};

template <>
struct PwmModeImpl<options::pwm_mode::ActiveHigh> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b110;
};

template <>
struct PwmModeImpl<options::pwm_mode::ActiveLow> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b111;
};

template <class Peripheral, class Config>
struct Pwm<mcu::policy::STM32F1Policy, Peripheral, Config> {
private:
  static inline uint32_t period = 0;

  using basic = PwmConfig<Peripheral, Config>;

  template <std::uint32_t Clock, std::uint32_t Frequency, bool AutoPeriod>
  struct PrescalerARR {
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
      static_assert(Clock % Frequency == 0 || AutoPeriod, "timer::Pwm: can not find prescaler and period");
      return std::pair<uint32_t, uint32_t>{0xFFFFFFFF, 0xFFFFFFFF};
    }

    static constexpr auto value = solve();

    static constexpr std::uint32_t prescaler = value.first;
    static constexpr std::uint32_t period    = value.second;
  };

  using channels = basic::channels;

public:
  template <class ClockConfig>
  static void apply() {
    using clock_sys                = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    using clock_bus                = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    constexpr auto m               = clock_bus::frequency == clock_sys::frequency ? 1 : 2;
    constexpr auto timer_frequency = clock_bus::frequency * m;
    constexpr auto pwm_mode        = PwmModeImpl<typename basic::pwm_mode>::value;

    static_assert(PwmModeImpl<typename basic::pwm_mode>::valid, "timer::Pwm: invalid PWM mode");

    Peripheral::CR1::CEN::reset();
    using psc_arr               = PrescalerARR<timer_frequency, basic::frequency::value, true>;

    constexpr auto initial_duty = (psc_arr::period * basic::initial_duty::value) / 100;
    constexpr auto timer_freq = timer_frequency / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));

    static_assert(timer_freq == basic::frequency::value,
                  "timer::Pwm: requested frequency cannot be generated exactly with PSC and ARR");

    Peripheral::PSC::write(psc_arr::prescaler);
    Peripheral::ARR::write(psc_arr::period);
    period = psc_arr::period;
    if constexpr (meta::mp_contains<channels, options::Channel<1>>::value) {
      Peripheral::CCMR1_Output::OC1M::write(pwm_mode);
      Peripheral::CCR1::write(initial_duty);
    }
    if constexpr (meta::mp_contains<channels, options::Channel<2>>::value) {
      Peripheral::CCMR1_Output::OC2M::write(pwm_mode);
      Peripheral::CCR2::write(initial_duty);
    }
    if constexpr (meta::mp_contains<channels, options::Channel<3>>::value) {
      Peripheral::CCMR2_Output::OC3M::write(pwm_mode);
      Peripheral::CCR3::write(initial_duty);
    }
    if constexpr (meta::mp_contains<channels, options::Channel<4>>::value) {
      Peripheral::CCMR2_Output::OC4M::write(pwm_mode);
      Peripheral::CCR4::write(initial_duty);
    }

    Peripheral::CNT::write(0);

    Peripheral::EGR::UG::set();

    Peripheral::SR::UIF::reset();
  }

  template <int channel>
  static void set_duty(uint8_t duty) {
    if (duty > 100) {
      return;
    }
    static_assert(channel >= 1 && channel <= 4,
                  "timer::Pwm: channel should be in range [1, 4]");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::Pwm: channel is not configured");

    uint16_t ccr_value = (period * duty) / 100;
    if constexpr (channel == 1) {
      Peripheral::CCR1::write(ccr_value);
    } else if constexpr (channel == 2) {
      Peripheral::CCR2::write(ccr_value);
    } else if constexpr (channel == 3) {
      Peripheral::CCR3::write(ccr_value);
    } else if constexpr (channel == 4) {
      Peripheral::CCR4::write(ccr_value);
    }
  }

  template <int channel>
  static void start_channel() {
    static_assert(channel >= 1 && channel <= 4,
                  "timer::Pwm: channel should be in range [1, 4]");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::Pwm: channel is not configured");

    if constexpr (channel == 1) {
      Peripheral::CC1E::set();
    } else if constexpr (channel == 2) {
      Peripheral::CC2E::set();
    } else if constexpr (channel == 3) {
      Peripheral::CC3E::set();
    } else if constexpr (channel == 4) {
      Peripheral::CC4E::set();
    }
  }

  template <int channel>
  static void stop_channel() {
    static_assert(channel >= 1 && channel <= 4,
                  "timer::Pwm: channel should be in range [1, 4]");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::Pwm: channel is not configured");

    if constexpr (channel == 1) {
      Peripheral::CC1E::reset();
    } else if constexpr (channel == 2) {
      Peripheral::CC2E::reset();
    } else if constexpr (channel == 3) {
      Peripheral::CC3E::reset();
    } else if constexpr (channel == 4) {
      Peripheral::CC4E::reset();
    }
  }

  static void start() {
    Peripheral::CR1::CEN::set();
  }

  static void stop() {
    Peripheral::CR1::CEN::reset();
  }
};
}