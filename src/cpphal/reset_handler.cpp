#include <cstdint>

#include "runtime/handlers.hpp"

extern "C" {
extern std::uint32_t _sidata;

extern std::uint32_t _sdata;
extern std::uint32_t _edata;

extern std::uint32_t _sbss;
extern std::uint32_t _ebss;

void __libc_init_array();
int  main();

void Reset_Handler() {
  //
  // Copy .data from FLASH to RAM
  //
  const std::uint32_t* src = &_sidata;
  std::uint32_t*       dst = &_sdata;

  while (dst < &_edata) {
    *dst++ = *src++;
  }

  //
  // Zero .bss
  //
  dst = &_sbss;

  while (dst < &_ebss) {
    *dst++ = 0;
  }

  //
  // Run global constructors
  //
  __libc_init_array();

  //
  // Run application
  //
  (void)main();

  //
  // main() should never return
  //
  while (true) {
  }
}
}