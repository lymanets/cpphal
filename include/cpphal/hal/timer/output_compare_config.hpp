#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"
#include "hal/timer/channel_config.hpp"
#include "hal/timer/options.hpp"

namespace hal::timer::config {

template <class... Options>
struct OutputCompare : core::OptionHolder<Options...> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

  template <class Tag>
  struct get_channel_tag {
    template <class T>
    using fn = impl::channel_get<T, typename T::tag>::type;
  };

  template <class Tag>
  struct get_tag {
    template <class T>
    using fn = typename T::tag;
  };

  template <class Tag>
  struct is_tag {
    template <class T>
    using fn = std::bool_constant<
      std::is_same_v<typename T::tag, Tag>
    >;
  };

  using Channels = meta::mp_copy_if_q<
    options_t,
    is_tag<tags::ChannelConfig>
  >;

public:
  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::Frequency>, options_t>,
        tags::Frequency>::value == 1,
      "timer::OutputCompare: Exactly one Frequency<> must be specified.");

  using NumberOfChannels = meta::mp_count<meta::mp_transform_q<get_channel_tag<tags::ChannelConfig>, options_t>,
                                          tags::Channel>;

  static_assert(
      NumberOfChannels::value > 0 && NumberOfChannels::value <= 4,
      "timer::OutputCompare: At least one Channel<> must be specified but no more than 4.");

  static_assert(
      meta::mp_size<Channels>::value == meta::mp_size<meta::mp_unique<Channels>>::value,
      "timer::OutputCompare: channel specified more than once"
      );
};
}