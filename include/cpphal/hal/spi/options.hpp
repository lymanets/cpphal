#pragma once

#include <cstdint>

#include "hal/option.hpp"

#include "tags.hpp"

namespace hal::spi::options {
namespace direction {
struct FullDuplex : Option<tags::Direction> {
};

struct TransmitOnly : Option<tags::Direction> {
};

struct ReceiveOnly : Option<tags::Direction> {
};

struct HalfDuplexTransmit : Option<tags::Direction> {
};

struct HalfDuplexReceive : Option<tags::Direction> {
};
}

template <std::uint32_t Value>
struct Baud : Option<tags::Baud> {
  static constexpr std::uint32_t value = Value;
};

template <std::uint8_t Bits>
struct DataBits : Option<tags::DataBits> {
  static constexpr std::uint8_t value = Bits;
};

namespace mode {
struct Master : Option<tags::Mode> {
};

struct Slave : Option<tags::Mode> {
};
}

namespace clock_polarity {
struct High : Option<tags::ClockPolarity> {
};

struct Low : Option<tags::ClockPolarity> {
};
}

namespace clock_phase {
struct FirstEdge : Option<tags::ClockPhase> {
};

struct SecondEdge : Option<tags::ClockPhase> {
};
}

namespace bit_order {
struct MSBFirst : Option<tags::BitOrder> {
};

struct LSBFirst : Option<tags::BitOrder> {
};
}

template <class... Args>
struct Events : Option<tags::Events> {
  using value = meta::mp_list<Args...>;
};
}