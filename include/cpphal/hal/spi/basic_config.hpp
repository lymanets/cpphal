#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "options.hpp"

namespace hal::spi {
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
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::Mode>, options_t>,
        tags::Mode>::value == 1,
      "Exactly one Mode<> must be specified.");

  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::Baud>, options_t>,
        tags::Baud>::value == 1,
      "Exactly one Baud<> must be specified.");

  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::DataBits>, options_t>,
        tags::DataBits>::value == 1,
      "Exactly one DataBits<> must be specified.");

  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::ClockPhase>, options_t>,
        tags::ClockPhase>::value == 1,
      "Exactly one ClockPhase<> must be specified.");

  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::ClockPolarity>, options_t>,
        tags::ClockPolarity>::value == 1,
      "Exactly one ClockPolarity must be specified.");

};
}