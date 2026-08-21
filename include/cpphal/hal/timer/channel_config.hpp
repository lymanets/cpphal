#pragma once
#include "meta/meta.hpp"

#include "hal/core/option_holder.hpp"

#include "hal/timer/options.hpp"

namespace hal::timer {
namespace tags {
struct ChannelConfig {
};
}

namespace impl {
template <class T, class Tag>
struct channel_get {
  using type = Tag;
};

template <class T>
struct channel_get<T, tags::ChannelConfig> {
  using type = typename T::template get<tags::Channel>::tag;
};
}

namespace config {
template <class... Options>
struct Channel : core::OptionHolder<Options...>, Option<tags::ChannelConfig> {
private:
  using options_t = typename core::OptionHolder<Options...>::options;

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
    is_tag<tags::Channel>
  >;

public:
  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::Initial>, options_t>,
        tags::Initial>::value == 1,
      "timer::Channel: Exactly one Initial<> must be specified.");

  static_assert(
      meta::mp_count<
        meta::mp_transform_q<get_tag<tags::Channel>, options_t>,
        tags::Channel>::value == 1,
      "timer::Channel: Exactly one Channel<> must be specified.");
};
}
}