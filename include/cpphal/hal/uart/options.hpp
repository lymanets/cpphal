#pragma once

#include <cstdint>

#include "hal/option.hpp"

#include "hal/uart/tags.hpp"

namespace hal::uart::options {
template <std::uint32_t Value>
struct Baud : Option<tags::Baud> {
  static constexpr std::uint32_t value = Value;
};

template <std::uint8_t Bits>
struct DataBits : Option<tags::DataBits> {
  static constexpr std::uint8_t value = Bits;
};

template <auto Bits>
struct StopBits : Option<tags::StopBits> {
  static constexpr auto value = Bits;
};

namespace parity {
struct None : Option<tags::Parity> {
};

struct Even : Option<tags::Parity> {
};

struct Odd : Option<tags::Parity> {
};
}

namespace direction {
struct Tx : Option<tags::Direction> {
};

struct Rx : Option<tags::Direction> {
};

struct TxRx : Option<tags::Direction> {
};
}

template <class... Args>
struct Events : Option<tags::Events> {
  using value = meta::mp_list<Args...>;
};

namespace flow_control {
struct None : Option<tags::FlowControl> {
};

struct Rts : Option<tags::FlowControl> {
};

struct Cts : Option<tags::FlowControl> {
};

struct RtsCts : Option<tags::FlowControl> {
};

}
}