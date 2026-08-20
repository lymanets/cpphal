#include <cpphal.hpp>

using namespace hal::literals;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<100_MHz>,
  hal::rcc::AHB<100_MHz>,
  hal::rcc::APB1<50_MHz>,
  hal::rcc::APB2<100_MHz>
>;

int main() {
  System::apply();
}