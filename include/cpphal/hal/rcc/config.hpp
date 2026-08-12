#pragma once

#include <cstdint>

#include "solver/solver.hpp"
#include "configurator/configurator.hpp"

#include "source_type.hpp"
#include "hal/option.hpp"
#include "tags.hpp"

namespace hal::rcc {
template <SourceType S, std::uint32_t Frequency>
struct Source {
  static constexpr auto source    = S;
  static constexpr auto frequency = Frequency;
};

template <std::uint32_t Frequency>
struct HSE : Source<SourceType::HSE, Frequency>, Option<tags::HSE> {
};

template <std::uint32_t Frequency>
struct HSI : Source<SourceType::HSI, Frequency>, Option<tags::HSI> {
};

template <std::uint32_t Frequency>
struct PLL : Source<SourceType::PLL, Frequency>, Option<tags::PLL> {
};

template <std::uint32_t Frequency>
struct Sysclk : Option<tags::Sysclk> {
  static constexpr std::uint32_t frequency = Frequency;
};

template <std::uint32_t Frequency>
struct AHB : Option<tags::AHB> {
  static constexpr std::uint32_t frequency = Frequency;
};

template <std::uint32_t Frequency>
struct APB1 : Option<tags::APB1> {
  static constexpr std::uint32_t frequency = Frequency;
};

template <std::uint32_t Frequency>
struct APB2 : Option<tags::APB2> {
  static constexpr std::uint32_t frequency = Frequency;
};

template <class Policy, class... Options>
struct Config {
  static void apply() {
    using Tree = Solver<Policy, Options...>;
    Configurator<Policy, Tree>::apply();
  }
};
}