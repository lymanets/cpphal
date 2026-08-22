#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/i2c/options.hpp"

namespace hal::i2c::config {
template <class... Options>
struct Basic : core::OptionHolder<Options...> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

  template <class Tag>
  struct get_tag {
    template <class T>
    using fn = typename T::tag;
  };

public:
  static_assert(
      core::get_option_count<options_t, tags::Frequency>::value == 1,
      "Exactly one Frequency<> must be specified.");
};
}