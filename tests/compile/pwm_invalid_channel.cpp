#include <cpphal.hpp>

using namespace hal::literals;

using PwmTimer = hal::timer::Pwm<
  3,
  hal::timer::config::Pwm<
    hal::timer::options::Frequency<20_kHz>,
    hal::timer::options::Channel<1>,
    hal::timer::options::InitialDuty<11>
  >
>;

int main() {
  PwmTimer::set_duty<5>(50);
}