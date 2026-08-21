#include <cpphal.hpp>


using Config = hal::gpio::Configurator<
  hal::gpio::PinDef<hal::gpio::port::A, 0, hal::gpio::config::OutputPushPull>,
  hal::gpio::PinDef<hal::gpio::port::A, 1, hal::gpio::config::Input>,
  hal::gpio::PinDef<hal::gpio::port::A, 2, hal::gpio::config::InputPullUp>,
  hal::gpio::PinDef<hal::gpio::port::A, 3, hal::gpio::config::Analog>
>;

int main() {
  Config::apply<meta::mp_list<>>();
}