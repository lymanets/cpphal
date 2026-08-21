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

template <int Instance, class Config>
struct Pwm<mcu::policy::STM32F1Policy, Instance, Config> {
private:
  using instance_t              = traits<tag<Instance>>;
  using Peripheral              = instance_t::peripheral;
  static inline uint32_t period = 0;

  using basic = PwmConfig<Peripheral, Config>;

  using channels = basic::channels;

public:
  template <class ClockConfig>
  static void apply() {
    using clock_sys                = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    using clock_bus                = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    constexpr auto m               = clock_bus::frequency == clock_sys::frequency ? 1 : 2;
    constexpr auto timer_frequency = clock_bus::frequency * m;

    using pwm_mode = PwmModeImpl<typename basic::pwm_mode>;
    static_assert(pwm_mode::valid, "timer::Pwm: invalid PWM mode");

    Peripheral::CR1::CEN::reset();
    using psc_arr = detail::PrescalerARR<timer_frequency, basic::frequency::value>;
    static_assert(
        psc_arr::valid,
        "timer::Pwm: requested frequency cannot be generated exactly");

    constexpr auto initial_duty = (psc_arr::period * basic::initial_duty::value) / 100;
    constexpr auto timer_freq   = timer_frequency / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));

    static_assert(timer_freq == basic::frequency::value,
                  "timer::Pwm: requested frequency cannot be generated exactly with PSC and ARR");

    Peripheral::PSC::write(psc_arr::prescaler);
    Peripheral::ARR::write(psc_arr::period);
    period = psc_arr::period;
    if constexpr (meta::mp_contains<channels, options::Channel<1>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC1E,
        typename Peripheral::CCER::CC1P,
        typename Peripheral::CCMR1_Output::OC1M,
        typename Peripheral::CCR1,
        pwm_mode,
        typename basic::polarity,
        initial_duty>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<2>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC2E,
        typename Peripheral::CCER::CC2P,
        typename Peripheral::CCMR1_Output::OC2M,
        typename Peripheral::CCR2,
        pwm_mode,
        typename basic::polarity,
        initial_duty>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<3>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC3E,
        typename Peripheral::CCER::CC3P,
        typename Peripheral::CCMR2_Output::OC3M,
        typename Peripheral::CCR3,
        pwm_mode,
        typename basic::polarity,
        initial_duty>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<4>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC4E,
        typename Peripheral::CCER::CC4P,
        typename Peripheral::CCMR2_Output::OC4M,
        typename Peripheral::CCR4,
        pwm_mode,
        typename basic::polarity,
        initial_duty>();
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
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::Pwm: channel should be in range");
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
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::Pwm: channel should be in range");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::Pwm: channel is not configured");

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

  template <int channel>
  static void stop_channel() {
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::Pwm: channel should be in range");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::Pwm: channel is not configured");

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

  static void start() {
    Peripheral::CR1::CEN::set();
  }

  static void stop() {
    Peripheral::CR1::CEN::reset();
  }
};
}