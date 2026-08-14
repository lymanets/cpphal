# STM32 C++20 HAL

A compile-time C++20 hardware abstraction layer for STM32 microcontrollers.

The current development target is **STM32F1**.

## Support Status

| Family | SVD | Registers | Policy | GPIO | RCC | UART | SPI | I2C | DMA | Timers | ADC | Status |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| STM32F0 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| **STM32F1** | ✅ | ✅ | ✅ | ✅ | ✅ | 🟡 | 🟡 | ❌ | ❌ | ❌ | ❌ | **Current target** |
| STM32F2 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32F3 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32F4 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32F7 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32G0 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32G4 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32H5 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32H7 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32L0 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32L1 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32L4 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |
| STM32U5 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | Not added |

Legend: `❌` not implemented · `🟡` partially implemented / in development
· `✅` implemented and tested on hardware.

## Documentation

- [Peripheral Usage](docs/peripheral-usage.md)
- [How the Library Works](docs/how-it-works.md)
- [Roadmap](docs/roadmap.md)
- [License](LICENSE.md)
