# UART

UART is the main high-level peripheral currently demonstrated by the project.

The current STM32F1 example configures USART1 with:

- 115200 baud
- 8 data bits
- 1 stop bit
- no parity
- TX + RX
- no flow control
- `RxNotEmpty` event

## Driver

The public configuration has this shape:

```cpp
using LogUart = hal::uart::Driver<
    1,
    BasicConfig,
    AdvancedConfig
>;
```

## Basic Configuration

```cpp
using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::options::Baud<115200>,
        hal::uart::options::DataBits<8>,
        hal::uart::options::StopBits<1>,
        hal::uart::options::parity::None,
        hal::uart::options::direction::TxRx
    >
>;
```

## Advanced Configuration

The current example adds flow control and an RX event:

```cpp
using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::options::Baud<115200>,
        hal::uart::options::DataBits<8>,
        hal::uart::options::StopBits<1>,
        hal::uart::options::parity::None,
        hal::uart::options::direction::TxRx
    >,
    hal::uart::Advanced<
        hal::uart::options::flow_control::None,
        hal::uart::options::Events<
            hal::core::On<
                hal::uart::events::RxNotEmpty,
                RxNotEmptyHandler
            >
        >
    >
>;
```

## Event Handler

The current example defines:

```cpp
struct RxNotEmptyHandler {

    static void run(std::uint8_t byte)
    {
        // process received byte
    }

};
```

The handler receives the received byte.

## Complete Board Example

```cpp
#pragma once

#include <cpphal.hpp>

using namespace hal::literals;

struct RxNotEmptyHandler {

    static void run(std::uint8_t byte)
    {
        // process byte
    }

};

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
    PinDef(A, 2, Input),
    PinDef(B, 1, Input),
    PinDef(C, 2, Input),
    PinDef(B, 2, Input)
>;

using LogUart = hal::uart::Driver<
    1,
    hal::uart::Basic<
        hal::uart::options::Baud<115200>,
        hal::uart::options::DataBits<8>,
        hal::uart::options::StopBits<1>,
        hal::uart::options::parity::None,
        hal::uart::options::direction::TxRx
    >,
    hal::uart::Advanced<
        hal::uart::options::flow_control::None,
        hal::uart::options::Events<
            hal::core::On<
                hal::uart::events::RxNotEmpty,
                RxNotEmptyHandler
            >
        >
    >
>;

using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;
```

Application:

```cpp
#include "board.hpp"

int main()
{
    BoardConfig::apply();

    LogUart::write("Hello world!\n");

    while (true) {
    }
}
```

## Options

### Baud rate
```cpp
hal::uart::options::BaudRate<115200>
```
### Data bits
```cpp
hal::uart::options::DataBits<8>
```
### Stop bits
```cpp
hal::uart::options::StopBits<1>
```
### Parity
```cpp
hal::uart::options::parity::None
hal::uart::options::parity::Even
hal::uart::options::parity::Odd
```
### Direction
```cpp
hal::uart::options::direction::Tx
hal::uart::options::direction::Rx
hal::uart::options::direction::TxRx
```
### Flow Control
```cpp
hal::uart::options::flow_control::None
hal::uart::options::flow_control::Rts
hal::uart::options::flow_control::Cts
hal::uart::options::flow_control::RtsCts
```
### Events
```cpp
hal::uart::events::RxNotEmpty
hal::uart::events::TxEmpty
hal::uart::events::TransmissionComplete
hal::uart::events::Idle
hal::uart::events::ParityError
hal::uart::events::Error
```

## Events

The current UART event configuration uses:

```cpp
hal::uart::options::Events<
    hal::core::On<Event, Handler>
>
```

## Transmit

After initialization:

```cpp
BoardConfig::apply();

LogUart::write("Hello world!\n");
```

This is the exact application-level transmit pattern used by the current STM32F1 UART example.

## Interrupts

The application does not manually define `USART1_IRQHandler()` or construct `.isr_vector`.

The UART event configuration supplies the driver's IRQ binding. The board configuration is then applied normally:

```cpp
BoardConfig::apply();
```