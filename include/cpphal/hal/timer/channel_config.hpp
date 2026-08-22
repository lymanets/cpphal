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

template <class options_t, class mode>
struct validate_required_options {
};

template <class options_t>
struct validate_required_options<options_t, options::mode::Output> {
  static_assert(
      core::get_option_count<options_t, tags::Initial>::value == 1,
      "timer::Channel: Exactly one Initial<> must be specified."
      );

  static_assert(
      core::get_option_count<options_t, tags::output_compare_mode>::value == 1,
      "timer::Channel: Exactly one output_compare_mode must be specified.");
};

template <class options_t>
struct validate_required_options<options_t, options::mode::Input> {
};
}

namespace config {
template <class... Options>
struct Channel : core::OptionHolder<Options...>, Option<tags::ChannelConfig> {
private:
  using parent = core::OptionHolder<Options...>;

  using options_t = typename parent::options;

  template <class Mode>
  using invalid_options_t = parent::template invalid_options<Mode, tags::mode>;

  template <class Tag>
  using get_t = parent::template get<Tag>;

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
      core::get_option_count<options_t, tags::mode>::value == 1,
      "timer::Channel: Exactly one mode must be specified."
      );

  static_assert(
      core::get_option_count<options_t, tags::Channel>::value == 1,
      "timer::Channel: Exactly one Channel<> must be specified.");

  static_assert(
      meta::mp_empty<invalid_options_t<get_t<tags::mode>>>::value,
      "timer::Channel: option is not valid for selected channel mode"
      );

  using s = impl::validate_required_options<options_t, get_t<tags::mode>>;
};
}
}