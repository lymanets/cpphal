#include <cpphal.hpp>

using PwmTimer = hal::timer::Pwm<
    3,
    hal::timer::config::Pwm<
        hal::timer::options::Frequency<20_kHz>,
        hal::timer::options::Channel<1>,
        hal::timer::options::Channel<2>,
        hal::timer::options::InitialDuty<50>
    >
>;

int main() {
    PwmTimer::apply();
    PwmTimer::set_duty<1>(25);
    PwmTimer::set_duty<2>(75);
    PwmTimer::start_channel<1>();
    PwmTimer::stop_channel<1>();
    PwmTimer::start();
    PwmTimer::stop();
}
