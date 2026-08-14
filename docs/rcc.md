# RCC

RCC configuration is supplied through `hal::SystemConfigurator`.

## STM32F1 Clock Configuration

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

The example therefore requests:

| Clock | Frequency |
|---|---:|
| HSE | 8 MHz |
| SYSCLK | 72 MHz |
| AHB | 72 MHz |
| APB1 | 36 MHz |
| APB2 | 72 MHz |

## Add to Board Configuration

```cpp
using BoardConfig = hal::Configurator<
    System,
    GpioConfig,
    LogUart
>;
```

Then:

```cpp
BoardConfig::apply();
```

## Clock Literals

The current API uses:

```cpp
using namespace hal::literals;
```

for literals such as:

```cpp
8_MHz
36_MHz
72_MHz
```
