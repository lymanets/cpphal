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

  template <auto P>
  struct CompareFn {
    template <class C>
    struct fn {
      static constexpr auto value = (P * C::value) / 100;
    };
  };

public:
  template <class ClockConfig>
  static void apply() {
    Peripheral::CR1::CEN::reset();
    using psc_arr = detail::PrescalerARR<ClockConfig, typename Peripheral::clock_tag, basic::frequency::value>;
    static_assert(
        psc_arr::valid,
        "timer::Pwm: requested frequency cannot be generated exactly");

    constexpr auto timer_freq = psc_arr::Clock / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));

    static_assert(timer_freq == basic::frequency::value,
                  "timer::Pwm: requested frequency cannot be generated exactly with PSC and ARR");

    Peripheral::PSC::write(psc_arr::prescaler);
    Peripheral::ARR::write(psc_arr::period);
    period = psc_arr::period;
    if constexpr (detail::has_channel_t<channels, 1>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC1E,
        typename Peripheral::CCER::CC1P,
        typename Peripheral::CCMR1_Output::OC1M,
        typename Peripheral::CCR1,
        typename detail::get_channel_t<channels, 1>::type,
        tags::output_compare_mode,
        PwmModeImpl,
        CompareFn<psc_arr::period>::template fn>();
    }
    if constexpr (detail::has_channel_t<channels, 2>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC2E,
        typename Peripheral::CCER::CC2P,
        typename Peripheral::CCMR1_Output::OC2M,
        typename Peripheral::CCR2,
        typename detail::get_channel_t<channels, 2>::type,
        tags::output_compare_mode,
        PwmModeImpl,
        CompareFn<psc_arr::period>::template fn>();
    }
    if constexpr (detail::has_channel_t<channels, 3>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC3E,
        typename Peripheral::CCER::CC3P,
        typename Peripheral::CCMR2_Output::OC3M,
        typename Peripheral::CCR3,
        typename detail::get_channel_t<channels, 3>::type,
        tags::output_compare_mode,
        PwmModeImpl,
        CompareFn<psc_arr::period>::template fn>();
    }
    if constexpr (detail::has_channel_t<channels, 4>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC4E,
        typename Peripheral::CCER::CC4P,
        typename Peripheral::CCMR2_Output::OC4M,
        typename Peripheral::CCR4,
        typename detail::get_channel_t<channels, 4>::type,
        tags::output_compare_mode,
        PwmModeImpl,
        CompareFn<psc_arr::period>::template fn>();
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

    uint16_t ccr_value = (period * duty) / 100;
    detail::set_compare<Peripheral, instance_t, channels, channel>(ccr_value);
  }

  template <int channel>
  static void start_channel() {
    detail::start_channel<Peripheral, instance_t, channels, channel>();
  }

  template <int channel>
  static void stop_channel() {
    detail::stop_channel<Peripheral, instance_t, channels, channel>();
  }

  static void start() {
    detail::start<Peripheral>();
  }

  static void stop() {
    detail::stop<Peripheral>();
  }
};
}