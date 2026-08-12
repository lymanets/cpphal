#pragma once

#include "boost/mp11.hpp"
#include "resolve_result.hpp"
#include "group_by.hpp"

namespace meta {
template <class Fn, class... Items>
using extract = mp_transform_q<Fn, Items...>;

struct EmptyList {
  using value = mp_list<>;
};

struct no_duplicate {

  static void test() {}
};

struct identity {
  template <class T>
  using fn = T;
};

template <class List, class KeyFn>
struct find_first_duplicate;

template <class KeyFn>
struct find_first_duplicate<mp_list<>, KeyFn> {
  using type = no_duplicate;
};

template <class Head, class... Tail, class KeyFn>
struct find_first_duplicate<mp_list<Head, Tail...>, KeyFn> {
private:
  using tail = mp_list<Tail...>;

  using key = mp_invoke_q<KeyFn, Head>;

  static constexpr bool found =
  (mp_count<
     mp_transform_q<KeyFn, tail>,
     key
   >::value != 0);

public:
  using type = std::conditional_t<
    found,
    Head,
    typename find_first_duplicate<tail, KeyFn>::type
  >;
};

template <class List, class KeyFn = identity>
using find_first_duplicate_t =
typename find_first_duplicate<List, KeyFn>::type;
}