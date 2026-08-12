#pragma once

#include "meta/meta.hpp"

namespace hal::core {
namespace internal {
struct configurator {
};
}

template <class KeyFn, class ValueFn, class... Configs>
struct ConfiguratorBase : internal::configurator {
  using Group = meta::group_by<KeyFn, ValueFn, meta::list<Configs...>>;
};

template <class Peripheral>
struct ConfiguratorSingleBase : internal::configurator {
  using Group = meta::mp_list<meta::Pair<Peripheral, void>>;
};


template <typename... Args>
concept IsAllConfigurators = (std::derived_from<Args, internal::configurator> && ...);

}