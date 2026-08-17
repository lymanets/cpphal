#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Device initialized!\n");
  LogUart::write("SPI example!\n");
  uint32_t version = pn532_t::getFirmwareVersion();

  while (true) {
    SystemTimer::delay_ms(1000);
    version++;
    LogUart::write("Ping!\n");
  }
}