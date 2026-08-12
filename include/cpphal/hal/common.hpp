#pragma once

#include <cstdint>
#include <boost/mp11.hpp>

namespace hal {
template <auto... Values>
struct PrintValues {
};

namespace mask {
template <class Reg, class Value>
struct RegisterUpdate {
  using reg   = Reg;
  using value = Value;

  static void apply() {
    reg::write(value());
  }
};

template <template<class> class Traits>
struct get_mask {
  template <class T>
  using fn = std::integral_constant<
    typename Traits<T>::value_type,
    Traits<T>::value
  >;
};

template <template<class> class Traits, class List>
using reduce_plus =
boost::mp11::mp_apply<
  boost::mp11::mp_plus,
  boost::mp11::mp_transform_q<
    get_mask<Traits>,
    List
  >
>;

template <template<class> class Traits, class Pair>
using pair_to_update =
RegisterUpdate<
  typename Pair::key,
  reduce_plus<Traits, typename Pair::values>
>;

template<template<class> class Traits>
struct pair_to_update_q
{
  template<class Pair>
  using fn = RegisterUpdate<
      typename Pair::key,
      reduce_plus<Traits, typename Pair::values>
  >;
};

}

}