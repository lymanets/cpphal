#include <cpphal.hpp>

using namespace hal::literals;

using SystemTimer = hal::timer::System<hal::timer::Unit::msec>;

using System = hal::SystemConfigurator<
  hal::rcc::PLL<8_MHz>,
  hal::rcc::Sysclk<72_MHz>,
  hal::rcc::AHB<72_MHz>,
  hal::rcc::APB1<36_MHz>,
  hal::rcc::APB2<72_MHz>
>;

using GpioConfig = hal::gpio::Configurator<>;

using PwmTimer = hal::timer::Pwm<
  3,
  hal::timer::config::Pwm<
    hal::timer::options::Frequency<20_kHz>,
    hal::timer::options::Channel<2>,
    hal::timer::options::Channel<2>,
    hal::timer::options::InitialDuty<20>,
    hal::timer::options::pwm_mode::ActiveHigh
  >
>;

using BoardConfig = hal::Configurator<
  System,
  SystemTimer,
  GpioConfig,
  PwmTimer
>;

int main() {
  return 0;
}