#pragma once

namespace hal {
template <class Tag>
struct Option {
  using tag = Tag;
};

template <class Map, class Value, class Default>
struct option_resolver {
  using type = meta::mp_map_find<Map, Value>;
};

template <class Map, class Default>
struct option_resolver<Map, void, Default> {
  using type = meta::mp_map_find<Map, Default>;
};

template <class Map, class Value, class Default>
using option_resolver_t = typename option_resolver<Map, Value, Default>::type;
}