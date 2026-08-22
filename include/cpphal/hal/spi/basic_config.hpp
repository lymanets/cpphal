#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/spi/options.hpp"

namespace hal::spi::config {
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
      core::get_option_count<options_t, tags::Mode>::value == 1,
      "Exactly one Mode<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::Baud>::value == 1,
      "Exactly one Baud<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::DataBits>::value == 1,
      "Exactly one DataBits<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::ClockPhase>::value == 1,
      "Exactly one ClockPhase<> must be specified.");

  static_assert(
      core::get_option_count<options_t, tags::ClockPolarity>::value == 1,
      "Exactly one ClockPolarity must be specified.");
};
}