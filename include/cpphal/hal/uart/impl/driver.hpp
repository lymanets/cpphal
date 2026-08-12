#pragma once

namespace hal::uart::impl {
template <
  class Policy,
  class Peripheral,
  class BasicConfig,
  class AdvancedConfig = void>
struct Driver {
};

}