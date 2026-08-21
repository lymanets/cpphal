#include <cpphal.hpp>

using namespace hal::literals;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using Pin = hal::gpio::PinDef<hal::gpio::port::A, 0, hal::gpio::config::Input>;

using Config = hal::gpio::Configurator<Pin>;

int main() {
  Config::apply<meta::mp_list<>>();
}