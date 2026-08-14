#pragma once

#include "device.hpp"
#include "hal/rcc/init/system_init.hpp"

namespace hal {
template <>
inline void SystemInit<mcu::policy::STM32F1Policy>() {
#if !defined(CLANG_TOOL_FIND_CONF)
  using namespace mcu;
  /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
  /* Set HSION bit */
  RCC::CR::ref() |= 0x00000001U;

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#if !defined(STM32F105xC) && !defined(STM32F107xC)
  RCC::CFGR::ref() &= 0xF8FF0000U;
#else
  RCC::CFGR::ref() &= 0xF0FF0000U;
#endif /* STM32F105xC */

  /* Reset HSEON, CSSON and PLLON bits */
  RCC::CR::ref() &= 0xFEF6FFFFU;

  /* Reset HSEBYP bit */
  RCC::CR::ref() &= 0xFFFBFFFFU;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC::CFGR::ref() &= 0xFF80FFFFU;

#if defined(STM32F105xC) || defined(STM32F107xC)
  /* Reset PLL2ON and PLL3ON bits */
  RCC::CR::ref() &= 0xEBFFFFFFU;

  /* Disable all interrupts and clear pending bits  */
  RCC::CIR::ref() = 0x00FF0000U;

  /* Reset CFGR2 register */
  RCC::CFGR2::ref() = 0x00000000U;
#elif defined(STM32F100xB) || defined(STM32F100xE)
  /* Disable all interrupts and clear pending bits  */
  RCC::CIR::ref() = 0x009F0000U;

  /* Reset CFGR2 register */
  RCC::CFGR2::ref() = 0x00000000U;
#else
  /* Disable all interrupts and clear pending bits  */
  RCC::CIR::ref() = 0x009F0000U;
#endif /* STM32F105xC */

#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM */
#endif

#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
#endif
}
}