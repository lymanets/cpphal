#pragma once

#include "base.hpp"

namespace hal::timer::impl {
using namespace literals;

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

template <int Instance, class Config>
struct OutputCompare<mcu::policy::STM32F1Policy, Instance, Config> {
private:
  using instance_t              = traits<tag<Instance>>;
  using Peripheral              = instance_t::peripheral;
  static inline uint32_t period = 0;

  using basic = OutputCompareConfig<Peripheral, Config>;

  using channels = basic::channels;

  template <auto P>
  struct CompareFn {
    template <class C, class M>
    struct fn {
      static constexpr auto value = std::is_same_v<M, options::output_compare_mode::PwmActiveHigh> ||
                                    std::is_same_v<M, options::output_compare_mode::PwmActiveLow>
                                      ? (P * C::value) / 100
                                      : C::value;
    };
  };

public:
  template <class ClockConfig>
  static void apply() {
    using psc_arr = detail::PrescalerARR<ClockConfig, typename Peripheral::clock_tag, basic::frequency::value>;
    static_assert(
        psc_arr::valid,
        "timer::OutputCompare: requested frequency cannot be generated exactly");
    // static_assert(
    //     basic::initial_compare::value <= psc_arr::period,
    //     "timer::OutputCompare: initial compare exceeds timer period");

    constexpr auto timer_freq = psc_arr::Clock / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));

    static_assert(
        timer_freq == basic::frequency::value,
        "timer::OutputCompare: requested frequency cannot be generated exactly with PSC and ARR");

    Peripheral::CR1::CEN::reset();
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
        OutputCompareModeImpl,
        CompareFn<psc_arr::period>>();
    }
    if constexpr (detail::has_channel_t<channels, 2>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC2E,
        typename Peripheral::CCER::CC2P,
        typename Peripheral::CCMR1_Output::OC2M,
        typename Peripheral::CCR2,
        typename detail::get_channel_t<channels, 2>::type,
        tags::output_compare_mode,
        OutputCompareModeImpl,
        CompareFn<psc_arr::period>>();
    }
    if constexpr (detail::has_channel_t<channels, 3>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC3E,
        typename Peripheral::CCER::CC3P,
        typename Peripheral::CCMR2_Output::OC3M,
        typename Peripheral::CCR3,
        typename detail::get_channel_t<channels, 3>::type,
        tags::output_compare_mode,
        OutputCompareModeImpl,
        CompareFn<psc_arr::period>>();
    }
    if constexpr (detail::has_channel_t<channels, 4>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC4E,
        typename Peripheral::CCER::CC4P,
        typename Peripheral::CCMR2_Output::OC4M,
        typename Peripheral::CCR4,
        typename detail::get_channel_t<channels, 4>::type,
        tags::output_compare_mode,
        OutputCompareModeImpl,
        CompareFn<psc_arr::period>>();
    }

    Peripheral::CNT::write(0);

    Peripheral::EGR::UG::set();

    Peripheral::SR::UIF::reset();
  }

  template <int channel>
  static void set_compare(uint16_t compare) {
    static_assert(detail::has_channel_t<channels, channel>::value,
                  "timer: channel is not configured");
    using channel_t = typename detail::get_channel_t<channels, channel>::type;
    using mode      = channel_t::template get<tags::output_compare_mode>;
    static_assert(!meta::is_same_v<mode,
                                  options::output_compare_mode::PwmActiveHigh,
                                  options::output_compare_mode::PwmActiveLow>,
                  "timer: channel is configured as PWM, use set_duty");
    // if (compare > period) {
    //   return;
    // }
    detail::set_compare<Peripheral, instance_t, channels, channel>(compare);
  }

  template <int channel>
  static void set_duty(uint8_t duty) {
    static_assert(detail::has_channel_t<channels, channel>::value,
                  "timer: channel is not configured");
    using channel_t = typename detail::get_channel_t<channels, channel>::type;
    using mode      = channel_t::template get<tags::output_compare_mode>;
    static_assert(meta::is_same_v<mode,
                                  options::output_compare_mode::PwmActiveHigh,
                                  options::output_compare_mode::PwmActiveLow>,
                  "timer: channel is not configured as PWM");
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