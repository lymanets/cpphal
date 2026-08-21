#include <cpphal.hpp>


using Config = hal::gpio::Configurator<
  hal::gpio::PinDef<hal::gpio::port::A, 0, hal::gpio::config::OutputPushPull>,
  hal::gpio::PinDef<hal::gpio::port::A, 0, hal::gpio::config::Input>
>;

int main() {
  Config::apply<meta::mp_list<>>();
}