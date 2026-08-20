#include <cpphal.hpp>

using namespace hal::literals;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using PwmTimer = hal::timer::Pwm<
  3,
  hal::timer::config::Pwm<
    hal::timer::options::Frequency<0>,
    hal::timer::options::InitialDuty<11>,
    hal::timer::options::Channel<1>
  >
>;

int main() {
  PwmTimer::apply<System>();
}