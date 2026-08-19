#pragma once

#include "types.hpp"

namespace meta {
template <class ValueFn, class List, class Index, bool C>
struct resolve_result_impl {
  using type = ValueFn::template fn<mp_at<List, Index>>;
};

template <class ValueFn, class List, class Index>
struct resolve_result_impl<ValueFn, List, Index, false> {
  using type = void;
};

template <class ValueFn, class List, bool C>
struct resolve_list_result_impl {
  using type = mp_copy_if_q<List, ValueFn>;
};

template <class ValueFn, class List>
struct resolve_list_result_impl<ValueFn, List, false> {
  using type = void;
};
}