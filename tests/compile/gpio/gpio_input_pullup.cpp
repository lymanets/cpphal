#include <cpphal.hpp>


using Pin = hal::gpio::PinDef<hal::gpio::port::A, 0, hal::gpio::config::InputPullUp>;

using Config = hal::gpio::Configurator<Pin>;

int main() {
  Config::apply<meta::mp_list<>>();
}