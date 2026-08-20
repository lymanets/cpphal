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
  2,
  hal::timer::config::Pwm<
    hal::timer::options::Frequency<20_kHz>,
    hal::timer::options::Channel<1>,
    hal::timer::options::Channel<2>,
    hal::timer::options::InitialDuty<50>
  >
>;

int main() {
  PwmTimer::apply<System>();
  PwmTimer::set_duty<1>(25);
  PwmTimer::set_duty<2>(75);
  PwmTimer::start_channel<1>();
  PwmTimer::stop_channel<1>();
  PwmTimer::start();
  PwmTimer::stop();
}