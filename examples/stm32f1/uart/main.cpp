#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("Hello world!\n");
  while (true) {
    SystemTimer::delay_ms(1000);
    LogUart::write("Ping!\n");
  }
}