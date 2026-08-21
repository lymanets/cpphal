#pragma once

#include "base.hpp"

namespace hal::timer::impl {
using namespace literals;

template <class Peripheral, class Config>
struct Basic<mcu::policy::STM32F1Policy, Peripheral, Config> {
private:
  using basic = BasicConfig<Peripheral, Config>;

public:
  template <class ClockConfig>
  static void apply() {
    using clock_sys                = ClockConfig::OptionHolder::template get<rcc::tags::Sysclk>;
    using clock_bus                = ClockConfig::OptionHolder::template get<typename Peripheral::clock_tag>;
    constexpr auto m               = clock_bus::frequency == clock_sys::frequency ? 1 : 2;
    constexpr auto timer_frequency = clock_bus::frequency * m;
    constexpr auto auto_period     = !std::is_same_v<typename basic::auto_period, void>;
    Peripheral::CR1::CEN::reset();
    using psc_arr = detail::PrescalerARR<timer_frequency,
                                         basic::frequency::value>;
    static_assert(
        timer_frequency % basic::frequency::value == 0 || auto_period,
        "timer::Basic: Frequency<> must divide the timer clock exactly; "
        "enable AutoPeriod to automatically calculate PSC and ARR");
    static_assert(
        psc_arr::valid,
        "timer::Basic: requested frequency cannot be generated");
    if constexpr (auto_period) {
      constexpr auto timer_freq = timer_frequency / ((psc_arr::prescaler + 1) * (psc_arr::period + 1));
      static_assert(timer_freq == basic::frequency::value,
                    "timer::Basic: Can not find prescaler and period");

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