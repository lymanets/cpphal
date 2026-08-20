#include <cpphal.hpp>

using namespace hal::literals;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using BasicTimer = hal::timer::Basic<
  2,
  hal::timer::config::Basic<
    hal::timer::options::Frequency<100_MHz>
  >
>;

int main() {
  BasicTimer::apply<System>();
}