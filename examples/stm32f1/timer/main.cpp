#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("Timer example!\n");

  BasicTimer::start();
  uint32_t c = 0;
  uint32_t d = 0;
  uint32_t duty = 10;
  PwmTimer::start();
  PwmTimer::start_channel<2>();
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
    PwmTimer::set_duty<2>(duty);
    duty += 10;
    if (duty > 100) {
      duty = 10;
    }
  }
}