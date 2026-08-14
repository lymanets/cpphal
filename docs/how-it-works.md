# How the Library Works

## Architecture

```text
                         Application
                              |
                              v
                     +-----------------+
                     |    Generic HAL  |
                     | GPIO / RCC      |
                     | UART / SPI      |
                     | I2C / DMA      |
                     | Timers / ADC    |
                     +--------+--------+
                              |
                              v
                     +-----------------+
                     |  Family Policy  |
                     | STM32F1Policy   |
                     | STM32F4Policy   |
                     | ...             |
                     +--------+--------+
                              |
                              v
                     +-----------------+
                     | Generated MCU   |
                     | Registers       |
                     | Fields          |
                     | Signals         |
                     | Events          |
                     | IRQs            |
                     +--------+--------+
                              |
                              v
                         STM32 Hardware
```

The generic HAL describes **what the application wants**. The family policy
describes **how the target MCU implements it**. Generated metadata describes
**what exists on the selected MCU**.

## Design Goals

- Zero-cost abstraction
- C++20 compile-time configuration
- Strong compile-time validation
- Generated MCU metadata from CMSIS-SVD
- Generic drivers independent of STM32 family details
- Direct MMIO in the resulting firmware

Invalid configurations should preferably fail during compilation:

- duplicate GPIO pins
- unsupported peripheral events
- unsupported peripheral signals
- invalid peripheral options
- invalid clock configuration
- unsupported peripheral features

## MCU Generator

The MCU generator converts CMSIS-SVD information into C++ types.

Generated information includes:

```text
CPU
Interrupts
Peripheral instances
Registers
Fields
Signals
Events
```

Typical generated peripheral structure:

```cpp
struct USART1 {
    struct CR1;
    struct CR2;
    struct CR3;
    struct SR;
    struct DR;

    struct signals;
    struct events;
};
```

Generated code should not be manually edited. Changes should be made in the
generator, SVD metadata, or family policy.

## Family Policies

Family-specific behavior is implemented through a policy:

```cpp
namespace mcu::policy {

struct STM32F1Policy {
};

}
```

Policies can define:

- clock configuration
- peripheral clock enable
- GPIO behavior
- alternate-function handling
- USART configuration
- SPI configuration
- DMA configuration
- timer configuration
- family-specific register behavior

## GPIO

Peripheral drivers do not directly configure GPIO pins.

```text
Peripheral
    |
    v
Generic signal requirements
    |
    v
resolve_signals_t
    |
    v
Peripheral::signals
    |
    v
PinDef
    |
    v
GPIO Configurator
```

Example generated signal:

```cpp
using Tx = hal::core::Signal<
    USART1,
    hal::core::SignalKind::Tx,
    hal::gpio::PinDef<
        hal::gpio::port::A,
        9,
        hal::gpio::config::AlternatePushPull<0>
    >
>;
```

The generic UART driver requests `Tx`, rather than hard-coding `PA9`.

## RCC

STM32F1 RCC configuration is solved at compile time.

```cpp
using System = hal::SystemConfigurator<
    hal::rcc::PLL<8_MHz>,
    hal::rcc::Sysclk<72_MHz>,
    hal::rcc::AHB<72_MHz>,
    hal::rcc::APB1<36_MHz>,
    hal::rcc::APB2<72_MHz>
>;
```

Current STM32F1 RCC work includes:

- PLL
- SYSCLK
- AHB
- APB1
- APB2
- PLL ready handling
- SYSCLK switching
- compile-time clock solving

# Interrupts

Interrupt handling is based on generated peripheral events.

Example handler:

```cpp
struct RxNotEmptyHandler {
    static void run(std::uint8_t value)
    {
        // process received byte
    }
};
```

Clear policies currently include:

```text
ReadSR_DR
WriteTC0
None
```

For `ReadSR_DR`:

```text
read SR
   |
   v
check pending flag
   |
   v
read DR
   |
   v
hardware flag cleared
```

The intended NVIC integration avoids requiring the application to manually
define every `USARTx_IRQHandler`.

The peripheral provides the IRQ number, while CMSIS provides:

```text
NVIC_SetVector()
NVIC_EnableIRQ()
```

# Development Rules

# Toolchain

Expected development environment:

```text
C++20
ARM GNU Toolchain
CMake
Boost.MP11
CMSIS-SVD
```

STM32F1 targets use:

```text
-mcpu=cortex-m3
-mthumb
```

# Code Size

The template-heavy architecture is intended to produce small firmware.

Compile-time configuration does not imply runtime overhead. Unused configuration, handlers and abstractions should be eliminated by the compiler/linker.
