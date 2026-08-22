#pragma once

#include "base.hpp"

namespace hal::timer::impl {
using namespace literals;

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
#if 1
    constexpr auto ccer  = detail::make_ccer<channels, typename Peripheral::CCER>();
    constexpr auto ccmr1 = detail::make_ccmr<channels,
                                             meta::mp_list<options::Channel<1>, options::Channel<2>>,
                                             typename Peripheral::CCMR1_Output,
                                             typename Peripheral::CCMR1_Input>();
    constexpr auto ccmr2 = detail::make_ccmr<channels,
                                             meta::mp_list<options::Channel<3>, options::Channel<4>>,
                                             typename Peripheral::CCMR2_Output,
                                             typename Peripheral::CCMR2_Input>();

    constexpr auto ccr1 = detail::make_ccr<channels, psc_arr, 1>();
    constexpr auto ccr2 = detail::make_ccr<channels, psc_arr, 2>();
    constexpr auto ccr3 = detail::make_ccr<channels, psc_arr, 3>();
    constexpr auto ccr4 = detail::make_ccr<channels, psc_arr, 4>();

    if constexpr (ccer != 0) Peripheral::CCER::write(ccer);
    if constexpr (ccmr1 != 0) Peripheral::CCMR1_Output::write(ccmr1);
    if constexpr (ccmr2 != 0) Peripheral::CCMR1_Output::write(ccmr2);
    if constexpr (ccr1 != 0) Peripheral::CCR1::write(ccr1);
    if constexpr (ccr2 != 0) Peripheral::CCR2::write(ccr2);
    if constexpr (ccr3 != 0) Peripheral::CCR3::write(ccr3);
    if constexpr (ccr4 != 0) Peripheral::CCR4::write(ccr4);
#else
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
#endif

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