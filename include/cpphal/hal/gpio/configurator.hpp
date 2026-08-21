#pragma once

#include "pin.hpp"
#include "hal/core/configurator.hpp"
#include "config.hpp"
#include "hal/common.hpp"

namespace hal::gpio {
namespace internal {
struct get_port {
  template <class T>
  using fn = typename T::port;
};

struct get_pin {
  template <class T>
  using fn = T;
};

template <class Port, uint8_t N>
struct PinIsNotConfigured {
};

template <class Pins, class Port, uint8_t N, class Index>
struct get_pin_impl;

template <class Pins, class Port, uint8_t N, std::size_t I>
struct get_pin_impl<Pins, Port, N, meta::mp_size_t<I>> {
  using item = meta::mp_at_c<Pins, I>;
  using type = Pin<Port, N, typename item::config, true>;
};

template <class Pins, class Port, uint8_t N>
struct get_pin_impl<Pins, Port, N, meta::mp_size<Pins>> {
  using type = PinIsNotConfigured<Port, N>;
};
}

template <class... Pins>
struct Configurator : core::ConfiguratorBase<internal::get_port, internal::get_pin, Pins...> {
  static_assert((internal::PinType<Pins> && ...), "gpio: Configurator accepts only gpio::Pin<...> types.");

private:
  template <class Port, uint8_t N>
  struct is_pin {
    template <class T>
    using fn = meta::mp_bool<
      std::is_same_v<typename T::port_tag, Port> &&
      (T::pin == N)
    >;
  };

  template <class Def>
  struct get_pin_impl {
    using type = Pin<typename Def::port_tag, Def::pin, typename Def::config, true>;
  };

  template <class Def>
  using get_pin_impl_t = typename get_pin_impl<Def>::type;

  template <class Defs>
  struct transform_to_pin_impl_t {
    using type = meta::mp_transform<
      get_pin_impl_t,
      Defs
    >;
  };

  using pins      = meta::mp_list<Pins...>;
  using pins_impl = transform_to_pin_impl_t<meta::mp_list<Pins...>>::type;

public:
  template <class Port, uint8_t N>
  using GetPin = internal::get_pin_impl<
    pins,
    Port,
    N,
    meta::mp_find_if_q<pins, is_pin<Port, N>>
  >::type;

  struct get_cr_reg {
    template <class T>
    using fn = typename T::CR_REG;
  };

  struct get_pin {
    template <class T>
    using fn = meta::mp_list<
      typename T::port,
      meta::mp_size_t<T::pin>
    >;
  };

  template <class T>
  struct mask_traits {
    using value_type                  = typename T::port::CRL::value_type;
    static constexpr value_type value = T::CR;
  };

  template <class Signals>
  static void apply() {
    using all_pins = meta::mp_append<
      typename transform_to_pin_impl_t<meta::mp_list<Pins...>>::type,
      typename transform_to_pin_impl_t<Signals>::type>;
    using duplicates = meta::find_duplicates_t<all_pins, get_pin>;
    static_assert(meta::mp_size<duplicates>::value == 0, "gpio: Pin is already configured");

    using Group = meta::group_by<get_cr_reg, internal::get_pin, all_pins>;
    using Regs  = meta::mp_transform_q<mask::pair_to_update_q<mask_traits>, Group>;

    meta::mp_for_each<Regs>([](auto x) {
      using Update = decltype(x);
      Update::apply();
    });
  }
};
}