#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/uart/options.hpp"

namespace hal::uart {

template <class... Options>
struct Advanced : core::OptionHolder<Options...> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

  template <class Tag>
  struct get_tag {
    template <class T>
    using fn = typename T::tag;
  };

public:
  // static_assert(
  //     meta::mp_count<
  //       meta::mp_transform_q<get_tag<tags::Interrupts>, options_t>,
  //       tags::Interrupts>::value == 1,
  //     "Exactly one Baud<> must be specified.");
  //
  // static_assert(
  //     meta::mp_count<
  //       meta::mp_transform_q<get_tag<tags::DataBits>, options_t>,
  //       tags::DataBits>::value == 1,
  //     "Exactly one DataBits<> must be specified.");
  //
  // static_assert(
  //     meta::mp_count<
  //       meta::mp_transform_q<get_tag<tags::StopBits>, options_t>,
  //       tags::StopBits>::value == 1,
  //     "Exactly one StopBits<> must be specified.");
  //
  // static_assert(
  //     meta::mp_count<
  //       meta::mp_transform_q<get_tag<tags::Parity>, options_t>,
  //       tags::Parity>::value == 1,
  //     "Exactly one Parity must be specified.");
  //
  // static_assert(
  //     meta::mp_count<
  //       meta::mp_transform_q<get_tag<tags::Direction>, options_t>,
  //       tags::Direction>::value == 1,
  //     "Exactly one direction must be specified.");
};
}