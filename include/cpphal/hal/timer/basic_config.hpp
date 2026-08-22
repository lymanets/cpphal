#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/timer/options.hpp"

namespace hal::timer::config {
template <class... Options>
struct Basic : core::OptionHolder<Options...> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

public:
  static_assert(
      core::get_option_count<options_t, tags::Frequency>::value == 1,
      "Exactly one Frequency<> must be specified.");
};
}