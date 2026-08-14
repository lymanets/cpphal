# Roadmap

# Roadmap

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
