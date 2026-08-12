#pragma once

#include <cstdint>
#include "hal/core/traits.hpp"

namespace hal::gpio {
template <class PortTag, uint8_t Pin, class Config>
struct PinDef {
  using port_tag               = PortTag;
  using port                   = typename traits<PortTag>::peripheral;
  static constexpr uint8_t pin = Pin;
  using config                 = Config;
};
}