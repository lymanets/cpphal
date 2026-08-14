# Configuration

The application describes the board as a type alias and applies the complete
configuration once.

## Board Configuration

```cpp
using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;
```

Then:

```cpp
int main()
{
    BoardConfig::apply();

    LogUart::write("Hello world!\n");

    while (true) {
    }
}
```

This is the structure used by the STM32F1 UART example.

## System

The current example uses:

```cpp
using namespace hal::literals;

using System = hal::SystemConfigurator<
    hal::rcc::PLL<8_MHz>,
    hal::rcc::Sysclk<72_MHz>,
    hal::rcc::AHB<72_MHz>,
    hal::rcc::APB1<36_MHz>,
    hal::rcc::APB2<72_MHz>
>;
```

## GPIO

The example defines GPIO configuration separately:

```cpp
using GpioConfig = hal::gpio::Configurator<
    PinDef(A, 0, OutputPushPull),
    PinDef(A, 1, Input),
    PinDef(A, 2, Input),
    PinDef(B, 1, Input),
    PinDef(C, 2, Input),
    PinDef(B, 2, Input)
>;
```

## Peripheral

A peripheral is another element of `hal::Configurator`:

```cpp
using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;
```

The application does not manually create the interrupt vector table.

## Complete Shape

```cpp
using System = /* system configuration */;

using GpioConfig = /* GPIO configuration */;

using Peripheral = /* peripheral configuration */;

using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    Peripheral
>;

int main()
{
    BoardConfig::apply();

    while (true) {
    }
}
```
