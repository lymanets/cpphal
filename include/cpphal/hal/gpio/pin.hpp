#pragma once

#include "types.hpp"
#include "hal/core/traits.hpp"

namespace hal::gpio {
namespace config {
template <Mode M,
          Pull P,
          OutputType T,
          Speed S,
          class AF>
struct Config {
  static constexpr Mode       mode  = M;
  static constexpr Pull       pull  = P;
  static constexpr OutputType type  = T;
  static constexpr Speed      speed = S;
  using AltFunction                 = AF;
};

// Input
using Input = Config<
  Mode::Input,
  Pull::None,
  OutputType::PushPull,
  Speed::Low,
  NoAlternate
>;

using InputPullUp = Config<
  Mode::Input,
  Pull::Up,
  OutputType::PushPull,
  Speed::Low,
  NoAlternate
>;

using InputPullDown = Config<
  Mode::Input,
  Pull::Down,
  OutputType::PushPull,
  Speed::Low,
  NoAlternate
>;

using Analog = Config<
  Mode::Analog,
  Pull::None,
  OutputType::PushPull,
  Speed::Low,
  NoAlternate
>;

using OutputPushPull = Config<
  Mode::Output,
  Pull::None,
  OutputType::PushPull,
  Speed::Low,
  NoAlternate
>;

using OutputOpenDrain = Config<
  Mode::Output,
  Pull::None,
  OutputType::OpenDrain,
  Speed::Low,
  NoAlternate
>;

template <uint8_t N>
using AlternatePushPull = Config<
  Mode::Alternate,
  Pull::None,
  OutputType::PushPull,
  Speed::Low,
  Alternate<N>
>;

template <uint8_t N>
using AlternateOpenDrain = Config<
  Mode::Alternate,
  Pull::None,
  OutputType::OpenDrain,
  Speed::Low,
  Alternate<N>
>;
}

struct PinTag {
};

template <class Policy, class Port, std::uint8_t N, class C, bool Enable>
struct PinImpl;

template <class Port, std::uint8_t Number, class C, bool Enable>
using Pin = PinImpl<typename traits<Port>::policy, Port, Number, C, Enable>;

namespace internal {
template <class T>
concept PinType = requires
{
  typename T::port;
};
}
} // namespace hal::gpio