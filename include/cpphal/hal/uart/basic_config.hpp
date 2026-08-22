#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/uart/options.hpp"

namespace hal::uart::config {
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
      core::get_option_count<options_t, tags::BaudRate>::value == 1,
      "Exactly one Baud<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::DataBits>::value == 1,
      "Exactly one DataBits<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::StopBits>::value == 1,
      "Exactly one StopBits<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::Parity>::value == 1,
      "Exactly one Parity must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::Direction>::value == 1,
      "Exactly one direction must be specified.");
};
}