#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("Timer example!\n");

  BasicTimer::start();

  uint32_t c = 0;
  while (true) {
    BasicTimer::delay_us(1000);
    c++;
    if (c <= 1'000) {
      continue;
    }
    c = 0;
    LogUart::write("Ping!\n");
  }
}