#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("I2C example!\n");

  while (true) {
    SystemTimer::delay_ms(1000);
    LogUart::write("Ping!\n");
  }
}