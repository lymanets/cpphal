#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("Timer example!\n");

  BasicTimer::start();
  uint32_t c    = 0;
  uint32_t d    = 0;
  uint32_t duty = 10;
  OCTimer::start();
  OCTimer::start_channel<1>();
  OCTimer::start_channel<2>();
  OCTimer::start_channel<3>();
  OCTimer::start_channel<4>();
  while (true) {
    BasicTimer::delay_us(1000);
    c++;
    if (c <= 1'000) {
      continue;
    }
    c = 0;
    LogUart::write("Ping!\n");
    d++;
    if (d <= 2) {
      continue;
    }
    d = 0;
    LogUart::write("Change PWM duty!\n");
    OCTimer::set_duty<2>(duty);
    OCTimer::set_compare<3>(duty);
    duty += 10;
    if (duty > 100) {
      duty = 10;
    }
  }
}