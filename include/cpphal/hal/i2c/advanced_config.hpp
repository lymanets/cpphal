#pragma once

#include "hal/core/option_holder.hpp"

namespace hal::i2c::config {
template <class... Options>
struct Advanced : core::OptionHolder<Options...> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

  template <class Tag>
  struct get_tag {
    template <class T>
    using fn = typename T::tag;
  };
};
}