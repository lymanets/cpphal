# STM32 C++20 HAL

A compile-time C++20 hardware abstraction layer for STM32 microcontrollers.

## Project Status

The project is currently focused on **STM32F1**.

```text
CMSIS-SVD
    |
    v
MCU Generator
    |
    +--> Registers
    +--> Fields
    +--> Interrupts
    +--> Signals
    +--> Events
    |
    v
Family Policy
    |
    v
Generic HAL
    |
    +--> GPIO
    +--> RCC
    +--> UART
    +--> SPI
    +--> I2C
    +--> DMA
    +--> Timers
    +--> ADC
```

## Support Matrix

Legend:

- `[ ]` — not implemented
- `[~]` — partially implemented / in development
- `[x]` — implemented
- `[T]` — implemented and tested on hardware

| Family | SVD | Registers | Policy | GPIO | RCC | UART | SPI | I2C | DMA | Timers | ADC | Status |
|---|---:|---:|---:|-----:|----:|-----:|---:|---:|---:|---:|---:|---|
| STM32F0 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| **STM32F1** | [x] | [x] | [x] |  [x] | [x] |  [~] | [~] | [ ] | [ ] | [ ] | [ ] | **Current target** |
| STM32F2 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32F3 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32F4 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32F7 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32G0 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32G4 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32H5 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32H7 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32L0 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32L1 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32L4 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |
| STM32U5 | [ ] | [ ] | [ ] |  [ ] | [ ] |  [ ] | [ ] | [ ] | [ ] | [ ] | [ ] | Not added |

A family is not considered fully supported merely because its SVD can be
parsed. The family policy, drivers, compile-time validation, and hardware
testing must also be completed.

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

## Configuration Model

Peripheral configuration is represented as a list of typed options.

```cpp
using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::Baud<115200>,
        hal::uart::DataBits<8>,
        hal::uart::StopBits<1>,
        hal::uart::None,
        hal::uart::TxRx
    >
>;
```

Value options contain a tag:

```cpp
template <std::uint32_t Value>
struct Baud : Option<tags::Baud> {
    static constexpr std::uint32_t value = Value;
};
```

Type-only options can be represented directly:

```cpp
namespace mode {

struct Master : Option<tags::Mode> {};
struct Slave  : Option<tags::Mode> {};

}
```

Missing options resolve to `void` and can be replaced with compile-time
defaults.

## Default Options

Generic option resolution supports defaults:

```cpp
template <class Map, class Value, class Default>
struct option_resolver {
    using type = meta::mp_map_find<Map, Value>;
};

template <class Map, class Default>
struct option_resolver<Map, void, Default> {
    using type = meta::mp_map_find<Map, Default>;
};
```

Example:

```cpp
using direction = resolve_option_t<
    Options,
    get<tags::Direction>,
    options::direction::FullDuplex
>;
```

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

# UART

UART is the first completed high-level peripheral driver.

### Basic configuration

Supported options:

```text
Baud
DataBits
StopBits
Parity
Direction
FlowControl
```

Example:

```cpp
using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::Baud<115200>,
        hal::uart::DataBits<8>,
        hal::uart::StopBits<1>,
        hal::uart::None,
        hal::uart::TxRx
    >
>;
```

### Direction

```text
Tx
Rx
TxRx
```

Signal requirements:

```text
Tx   -> TX
Rx   -> RX
TxRx -> TX + RX
```

### Flow control

Supported concepts:

```text
None
CTS
RTS
RTS/CTS
```

Flow-control configuration also resolves the corresponding GPIO signals.

### Events

Generic UART events:

```text
RxNotEmpty
TxEmpty
TransmissionComplete
Idle
ParityError
Error
```

Generated peripheral events contain their hardware-specific status and
interrupt-enable fields.

Example:

```cpp
using RxNotEmpty =
    hal::core::Event<
        hal::core::clear_policy::ReadSR_DR,
        CR1::RXNEIE,
        SR::RXNE
    >;
```

If the selected peripheral does not support an event, compilation fails.

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

# SPI

SPI is the next peripheral under active development.

### Basic options

```cpp
hal::spi::Basic<
    hal::spi::Baud<9_MHz>,
    hal::spi::DataBits<8>,
    hal::spi::mode::Master,
    hal::spi::clock_polarity::Low,
    hal::spi::clock_phase::FirstEdge,
    hal::spi::bit_order::MSBFirst
>
```

Options:

```text
Baud
DataBits
Mode
ClockPolarity
ClockPhase
BitOrder
Direction
```

### Direction

`FullDuplex` is the default.

```text
FullDuplex -> SCK + MOSI + MISO
TxOnly     -> SCK + MOSI
RxOnly     -> SCK + MISO
```

STM32F1 maps these through:

```text
CR1.BIDIMODE
CR1.BIDIOE
CR1.RXONLY
```

Generic SPI does not contain this STM32F1-specific register knowledge.

### Data width

STM32F1 supports:

```text
DataBits<8>
DataBits<16>
```

mapped to `CR1.DFF`.

### Clock

- `Low` / `High` -> `CR1.CPOL`
- `FirstEdge` / `SecondEdge` -> `CR1.CPHA`
- `MSBFirst` / `LSBFirst` -> `CR1.LSBFIRST`
- `Baud<N>` -> STM32F1 `BR[2:0]` divider

### Transfer

SPI is full duplex. Writing `DR` transmits while simultaneously receiving.

```cpp
while (!Peripheral::SR::TXE::read()) {
}

Peripheral::DR::write(data);

while (!Peripheral::SR::RXNE::read()) {
}

return Peripheral::DR::read();
```

# DMA

DMA is intentionally postponed until the generic peripheral abstraction is
stable.

The planned abstraction separates:

### Request

Who wants DMA?

```cpp
dma::Request<
    USART1,
    dma::Rx
>
```

### Configuration

How should the transfer work?

```text
Direction
PeripheralDataSize
MemoryDataSize
PeripheralIncrement
MemoryIncrement
Mode
Priority
```

Example:

```cpp
dma::Config<
    dma::PeripheralToMemory,
    dma::Byte,
    dma::Byte,
    dma::Fixed,
    dma::Increment,
    dma::Circular,
    dma::High
>
```

### Hardware resolution

```text
USART1::Rx
      |
      v
DMA request mapping
      |
      v
DMA1 / Channel5
      |
      v
STM32F1 policy
      |
      +--> CCR
      +--> CNDTR
      +--> CPAR
      +--> CMAR
```

The application should not normally specify the STM32F1 channel manually.

# Top-Level Configuration

System, GPIO and peripheral configurations are combined:

```cpp
using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;

BoardConfig::apply();
```

Example:

```cpp
using System = hal::SystemConfigurator<
    hal::rcc::PLL<8_MHz>,
    hal::rcc::Sysclk<72_MHz>,
    hal::rcc::AHB<72_MHz>,
    hal::rcc::APB1<36_MHz>,
    hal::rcc::APB2<72_MHz>
>;

using GpioConfig = hal::gpio::Configurator<
    PinDef(A, 0, OutputPushPull),
    PinDef(A, 1, Input),
    PinDef(B, 1, Input),
    PinDef(C, 2, Input)
>;

using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::Baud<115200>,
        hal::uart::DataBits<8>,
        hal::uart::StopBits<1>,
        hal::uart::None,
        hal::uart::TxRx
    >
>;

using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;

int main()
{
    BoardConfig::apply();
    LogUart::write("Hello\r\n");

    while (true) {
    }
}
```

# Roadmap

## Phase 1 — Core

- [x] C++20 foundation
- [x] MMIO register abstraction
- [x] Register fields
- [x] Access policies
- [x] Compile-time option system
- [x] Boost.MP11 integration
- [x] Compile-time validation

## Phase 2 — MCU Generator

- [x] CMSIS-SVD parsing
- [x] CPU generation
- [x] Peripheral generation
- [x] Register generation
- [x] Field generation
- [x] Interrupt generation
- [x] Signal generation
- [x] Event generation
- [ ] DMA request metadata
- [ ] Complete cross-family metadata

## Phase 3 — STM32F1 Foundation

- [x] GPIO
- [x] RCC
- [x] Clock solver
- [x] Peripheral clock configuration
- [x] Interrupt metadata
- [x] NVIC integration

## Phase 4 — UART

- [x] Basic configuration
- [x] Baud rate
- [x] Data bits
- [x] Stop bits
- [x] Parity
- [x] TX
- [x] RX
- [x] TX/RX
- [x] Flow control
- [x] Signal resolution
- [x] GPIO integration
- [x] Events
- [x] Interrupts
- [x] NVIC vector binding
- [ ] DMA integration
- [ ] LIN
- [ ] IrDA
- [ ] Smartcard

## Phase 5 — SPI

- [~] Basic configuration
- [~] Master/slave
- [~] Full duplex
- [~] TX only
- [~] RX only
- [~] Clock polarity
- [~] Clock phase
- [~] Bit order
- [~] 8/16-bit data
- [~] Baud rate
- [~] Signal resolution
- [ ] Blocking driver
- [ ] NSS configuration
- [ ] Events
- [ ] Interrupts
- [ ] CRC
- [ ] Half duplex
- [ ] TI mode
- [ ] DMA

## Phase 6 — I2C

- [ ] Basic configuration
- [ ] Master
- [ ] Slave
- [ ] Addressing
- [ ] Signals
- [ ] Events
- [ ] Interrupts
- [ ] Error handling
- [ ] DMA

## Phase 7 — Timers

- [ ] Basic timer
- [ ] Counter
- [ ] Prescaler
- [ ] ARR
- [ ] Input capture
- [ ] Output compare
- [ ] PWM
- [ ] One-pulse mode
- [ ] Encoder mode
- [ ] Timer interrupts
- [ ] Timer signal resolution

## Phase 8 — ADC

- [ ] ADC configuration
- [ ] Channels
- [ ] Sampling time
- [ ] Resolution
- [ ] Trigger configuration
- [ ] Conversion
- [ ] Interrupts
- [ ] DMA

## Phase 9 — DMA

- [ ] Generic DMA abstraction
- [ ] Request
- [ ] Direction
- [ ] Data size
- [ ] Memory increment
- [ ] Peripheral increment
- [ ] Normal mode
- [ ] Circular mode
- [ ] Priority
- [ ] Controller abstraction
- [ ] Channel abstraction
- [ ] Request mapping
- [ ] STM32F1 implementation
- [ ] UART integration
- [ ] SPI integration

## Phase 10 — Additional STM32 Families

- [ ] STM32F0
- [ ] STM32F2
- [ ] STM32F3
- [ ] STM32F4
- [ ] STM32F7
- [ ] STM32G0
- [ ] STM32G4
- [ ] STM32H5
- [ ] STM32H7
- [ ] STM32L0
- [ ] STM32L1
- [ ] STM32L4
- [ ] STM32U5

# Recommended Implementation Order

```text
Core
  |
  +--> Generator
  |
  +--> GPIO
  |
  +--> RCC
  |
  +--> UART
  |      |
  |      +--> Signals
  |      +--> Events
  |      +--> Interrupts
  |      +--> Flow Control
  |
  +--> SPI
  |
  +--> I2C
  |
  +--> Timers
  |
  +--> ADC
  |
  +--> DMA
  |
  +--> Additional MCU Families
```

# Adding a New STM32 Family

1. Add the CMSIS-SVD files.
2. Add the family to the generator.
3. Generate CPU, peripheral, register, field and IRQ metadata.
4. Generate or define signals and events.
5. Create the family policy.
6. Implement family-specific differences.
7. Enable generic drivers where compatible.
8. Add compile-time validation.
9. Test on real hardware.
10. Update the support matrix.

A family should remain `[~]` until its core functionality has been validated.

# Development Rules

## Generic code

Generic HAL code must not contain assumptions about a specific STM32 family.

## Family-specific code

Family-specific behavior belongs in the corresponding family policy or
peripheral specialization.

Example:

```text
generic direction
       |
       v
STM32F1 specialization
       |
       +--> CR1.BIDIMODE
       +--> CR1.BIDIOE
       +--> CR1.RXONLY
```

## Generated code

Generated files should not be manually modified.

Modify:

```text
SVD
Generator
Metadata
Family Policy
```

and regenerate.

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

# Project Completion Criteria

The STM32F1 baseline is complete when:

- [x] Core MMIO abstraction is stable
- [x] SVD generator is usable
- [x] GPIO is functional
- [x] RCC clock solver is functional
- [x] UART is functional
- [x] UART flow control is functional
- [x] UART events are functional
- [x] UART interrupts are functional
- [ ] SPI is functional
- [ ] I2C is functional
- [ ] Timer support is functional
- [ ] ADC support is functional
- [ ] DMA abstraction is functional
- [ ] DMA integration is functional
- [ ] Additional STM32 families are supported

# License

```text
MIT License

Copyright (c) 2026 Alex Lymanets

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```