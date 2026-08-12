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
}