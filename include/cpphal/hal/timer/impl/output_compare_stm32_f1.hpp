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
struct OutputCompareModeImpl<void> {
  // Default value
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b000;
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

template <int Instance, class Config>
struct OutputCompare<mcu::policy::STM32F1Policy, Instance, Config> {
private:
  using instance_t              = traits<tag<Instance>>;
  using Peripheral              = instance_t::peripheral;
  static inline uint32_t period = 0;

  using basic = OutputCompareConfig<Peripheral, Config>;

  using channels = basic::channels;

public:
  template <class ClockConfig>
  static void apply() {
    using oc_mode = OutputCompareModeImpl<typename basic::oc_mode>;

    static_assert(
        oc_mode::valid,
        "timer::OutputCompare: invalid OC mode");

    using psc_arr = detail::PrescalerARR<ClockConfig, typename Peripheral::clock_tag, basic::frequency::value>;
    static_assert(
        psc_arr::valid,
        "timer::OutputCompare: requested frequency cannot be generated exactly");
    static_assert(
        basic::initial_compare::value <= psc_arr::period,
        "timer::OutputCompare: initial compare exceeds timer period");

    constexpr auto timer_freq = psc_arr::Clock / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));

    static_assert(
        timer_freq == basic::frequency::value,
        "timer::OutputCompare: requested frequency cannot be generated exactly with PSC and ARR");

    Peripheral::CR1::CEN::reset();
    Peripheral::PSC::write(psc_arr::prescaler);
    Peripheral::ARR::write(psc_arr::period);
    period = psc_arr::period;
    if constexpr (meta::mp_contains<channels, options::Channel<1>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC1E,
        typename Peripheral::CCER::CC1P,
        typename Peripheral::CCMR1_Output::OC1M,
        typename Peripheral::CCR1,
        oc_mode,
        typename basic::polarity,
        basic::initial_compare::value>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<2>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC2E,
        typename Peripheral::CCER::CC2P,
        typename Peripheral::CCMR1_Output::OC2M,
        typename Peripheral::CCR2,
        oc_mode,
        typename basic::polarity,
        basic::initial_compare::value>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<3>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC3E,
        typename Peripheral::CCER::CC3P,
        typename Peripheral::CCMR2_Output::OC3M,
        typename Peripheral::CCR3,
        oc_mode,
        typename basic::polarity,
        basic::initial_compare::value>();
    }
    if constexpr (meta::mp_contains<channels, options::Channel<4>>::value) {
      detail::configure_channel<
        typename Peripheral::CCER::CC4E,
        typename Peripheral::CCER::CC4P,
        typename Peripheral::CCMR2_Output::OC4M,
        typename Peripheral::CCR4,
        oc_mode,
        typename basic::polarity,
        basic::initial_compare::value>();
    }

    Peripheral::CNT::write(0);

    Peripheral::EGR::UG::set();

    Peripheral::SR::UIF::reset();
  }

  template <int channel>
  static void set_compare(uint16_t compare) {
    // if (compare > period) {
    //   return;
    // }
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::OutputCompare: channel should be in range");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
                  "timer::OutputCompare: channel is not configured");

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

  template <int channel>
  static void start_channel() {
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::OutputCompare: channel should be in range");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
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

  template <int channel>
  static void stop_channel() {
    static_assert(channel >= 1 && channel <= instance_t::channels,
                  "timer::OutputCompare: channel should be in range");
    static_assert(meta::mp_contains<channels, options::Channel<channel>>::value,
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

  static void start() {
    Peripheral::CR1::CEN::set();
  }

  static void stop() {
    Peripheral::CR1::CEN::reset();
  }
};
}