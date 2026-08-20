#pragma once

#include <device.hpp>


#include "hal/literals.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/rcc/solver/solver_base.hpp"
#include "hal/rcc/source_type.hpp"
#include "hal/rcc/tags.hpp"

namespace hal::rcc {
template <class... Options>
struct Solver<mcu::policy::STM32F1Policy, Options...> {
private:
  using OptionHolder = core::OptionHolder<Options...>;

  using Source  = OptionHolder::template get<tags::PLL>;
  using SYSCLK  = OptionHolder::template get<tags::Sysclk>;
  using AHBCLK  = OptionHolder::template get<tags::AHB>;
  using APB1CLK = OptionHolder::template get<tags::APB1>;
  using APB2CLK = OptionHolder::template get<tags::APB2>;

public:
  using policy = mcu::policy::STM32F1Policy;

  static constexpr std::uint32_t src = Source::frequency;
  static constexpr std::uint32_t sys = SYSCLK::frequency;

  static consteval int pllmul() {
    for (int m = 2; m <= 16; ++m) if (src * m == sys) return m;

    return -1;
  }

  static consteval std::uint8_t pllmul_bits() {
    constexpr int mul = pllmul();

    static_assert(mul != -1, "rcc: PLL multiplier not found.");

    // RM0008:
    // x2=0000 ... x16=1110
    return static_cast<std::uint8_t>(mul - 2);
  }

  static consteval std::uint8_t sw_bits() {
    constexpr auto s = Source::source;
    if constexpr (s == SourceType::PLL) return 0b10;
    else if constexpr (s == SourceType::HSE) return 0b01;
    else if constexpr (s == SourceType::HSI) return 0b00;
    else static_assert(s >= 3, "rcc: Invalid SourceType.");
    return 0;
  }

  static consteval std::uint8_t hpre_bits() {
    constexpr std::uint32_t f = AHBCLK::frequency;

    static_assert(f == sys, "rcc: Invalid AHB frequency. !");

    if constexpr (f == sys) return 0b0000;
    else if constexpr (f == sys / 2) return 0b1000;
    else if constexpr (f == sys / 4) return 0b1001;
    else if constexpr (f == sys / 8) return 0b1010;
    else if constexpr (f == sys / 16) return 0b1011;
    else if constexpr (f == sys / 64) return 0b1100;
    else if constexpr (f == sys / 128) return 0b1101;
    else if constexpr (f == sys / 256) return 0b1110;
    else if constexpr (f == sys / 512) return 0b1111;
    else static_assert(f == 0, "rcc: Invalid AHB frequency.");
    return 0;
  }

  static consteval std::uint8_t ppre_bits(std::uint32_t bus) {
    if (bus == sys) return 0b000;
    if (bus == sys / 2) return 0b100;
    if (bus == sys / 4) return 0b101;
    if (bus == sys / 8) return 0b110;
    if (bus == sys / 16) return 0b111;

    return 0xff;
  }

  static consteval std::uint8_t flash_latency() {
    using namespace literals;

    if constexpr (sys <= 24_MHz) return 0;
    else if constexpr (sys <= 48_MHz) return 1;
    else return 2;
  }

  using FLASH     = mcu::FLASH;
  using ACRField  = FLASH::ACR;
  using RCC       = mcu::RCC;
  using CFGRField = RCC::CFGR;

  static_assert(sys <= policy::max_sysclk, "rcc: Invalid Sysclk frequency");
  static_assert(APB1CLK::frequency <= policy::max_apb1, "rcc: Invalid APB1 frequency");
  static_assert(APB2CLK::frequency <= policy::max_apb2, "rcc: Invalid APB2 frequency");

  static constexpr std::uint8_t PLLSRC   = 1; // HSE
  static constexpr std::uint8_t PLLXTPRE = 0; // HSE not divided
  static constexpr std::uint8_t PLLMUL   = pllmul_bits();

  static constexpr std::uint8_t HPRE  = hpre_bits();
  static constexpr std::uint8_t PPRE1 = ppre_bits(APB1CLK::frequency);
  static constexpr std::uint8_t PPRE2 = ppre_bits(APB2CLK::frequency);

  static_assert(PPRE1 != 0xff, "rcc: Invalid APB1 frequency.");
  static_assert(PPRE2 != 0xff, "rcc: Invalid APB2 frequency.");

  static constexpr std::uint8_t FLASH_LATENCY = flash_latency();

  static constexpr std::uint8_t SW = 0b10; // PLL

  static constexpr CFGRField::value_type CFGR =
      CFGRField::HPRE::encode(HPRE) |
      CFGRField::PPRE1::encode(PPRE1) |
      CFGRField::PPRE2::encode(PPRE2) |
      CFGRField::PLLSRC::encode(PLLSRC) |
      CFGRField::PLLMUL::encode(PLLMUL);

  static constexpr ACRField::value_type ACR = ACRField::LATENCY::encode(FLASH_LATENCY)
                                              | ACRField::PRFTBE::encode(1);

  static void apply() {
  }
};
}