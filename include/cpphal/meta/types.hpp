#pragma once

#include "boost/mp11.hpp"

namespace meta {
using namespace boost::mp11;

template <class... T>
using list = mp_list<T...>;
}