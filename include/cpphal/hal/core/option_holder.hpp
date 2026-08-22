#pragma once

#include "meta/meta.hpp"

namespace hal::core {
template <class Tag>
struct get_tag {
  template <class T>
  using fn = typename T::tag;
};

template <class Options, class Tag>
struct get_option_count {
  static constexpr bool value = meta::mp_count<meta::mp_transform_q<get_tag<Tag>, Options>, Tag>::value;
};

template <class... Options>
struct OptionHolder {
protected:
  using options = meta::mp_list<Options...>;

  template <class Tag>
  struct has_tag {
    template <class T>
    using fn = std::bool_constant<
      std::is_same_v<typename T::tag, Tag>
    >;
  };

  template <class Tag>
  struct get_impl {
    struct get_value {
      template <class T>
      using fn = T;
    };

    using index                 = meta::mp_find_if_q<options, has_tag<Tag>>;
    static constexpr bool found = index::value < meta::mp_size<options>::value;
    using type                  = meta::resolve_result_impl<get_value, options, index, found>::type;
  };

  template <class Tag>
  struct get_list_impl {
    struct get_value {
      template <class T>
      using fn = std::bool_constant<
        std::is_same_v<typename T::tag, Tag>
      >;
    };

    using index                 = meta::mp_find_if_q<options, has_tag<Tag>>;
    static constexpr bool found = index::value < meta::mp_size<options>::value;
    using type                  = meta::resolve_list_result_impl<get_value, options, found>::type;
  };

public:
  template <class Tag>
  using get = get_impl<Tag>::type;

  template <class Tag>
  using get_list = get_list_impl<Tag>::type;
};
}