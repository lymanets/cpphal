#include "board.hpp"

int main() {
  BoardConfig::apply();

  LogUart::write("Hello world!\n");

  while (true) {
  }
}