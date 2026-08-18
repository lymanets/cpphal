#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("Timer example!\n");
  while (true) {
    SystemTimer::delay_ms(1000);
    LogUart::write("Ping!\n");
  }
}