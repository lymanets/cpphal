# GPIO

GPIO configuration is provided through `hal::gpio::Configurator`.

## Pin Definition

The current STM32F1 examples use:

```cpp
PinDef(PORT, PIN, CONFIG)
```

For example:

```cpp
PinDef(A, 0, OutputPushPull)
PinDef(A, 1, Input)
```

## GPIO Configurator

```cpp
using GpioConfig = hal::gpio::Configurator<
    PinDef(A, 0, OutputPushPull),
    PinDef(A, 1, Input),
    PinDef(A, 2, Input)
>;
```

## Board Configuration

Combine it with the system and peripherals:

```cpp
using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;
```

Apply once:

```cpp
BoardConfig::apply();
```

## Peripheral Pins

When a peripheral requires signals such as UART TX/RX, those requirements are
part of the peripheral configuration. The application should not reproduce
the peripheral's register-level configuration in GPIO code.

Use the GPIO configurator for application-level pins and the peripheral
configuration for peripheral signals.
