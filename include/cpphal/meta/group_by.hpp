#pragma once

#include "types.hpp"
#include "resolve_result.hpp"
#include <type_traits>

namespace meta {
using namespace boost::mp11;

template <class Key, class Value>
struct Pair {
  using key    = Key;
  using values = Value;
};

template <class KeyFn, class ValueFn>
struct make_pair {
  template <class T>
  using fn = Pair<
    typename KeyFn::template fn<T>,
    list<typename ValueFn::template fn<T>>
  >;
};

template <class ExistingPair, class NewPair>
using merge_pair = Pair<
  typename ExistingPair::key,
  mp_append<
    typename ExistingPair::values,
    typename NewPair::values
  >
>;

template <class Map, class NewPair, class Found>
struct map_inserter_impl;

template <class Map, class NewPair>
struct map_inserter_impl<Map, NewPair, void> {
  using type = mp_push_back<Map, NewPair>;
};

template <class Map, class NewPair, class Found>
struct map_inserter_impl {
  using type = mp_map_replace<
    Map,
    merge_pair<Found, NewPair>
  >;
};

template <class Map, class NewPair>
struct map_inserter
    : map_inserter_impl<
      Map,
      NewPair,
      mp_map_find<Map, typename NewPair::key>
    > {
};

template <typename Map, typename NewPair>
using map_inserter_t = typename map_inserter<Map, NewPair>::type;

template <class KeyFn, class ValueFn, class List>
using group_by = mp_fold<
  mp_transform_q<make_pair<KeyFn, ValueFn>, List>,
  list<>,
  map_inserter_t
>;

struct void_fallback {
  using types = void;
};

template <class Key>
struct get_by_key {
  template <class Pair>
  using fn = std::is_same<typename Pair::key, Key>;
};

template <class List, class Key>
struct get_by_key_impl {
  using index = mp_find_if_q<List, get_by_key<Key>>;

  static constexpr bool found = index::value < mp_size<List>::value;

  struct get_values {
    template <class T>
    using fn = T::values;
  };

  using type = resolve_result_impl<get_values, List, index, found>::type;
};

template <class List, class Key>
using get_by_key_t = typename get_by_key_impl<List, Key>::type;
}