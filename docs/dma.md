# DMA

DMA is not currently a completed public peripheral API on the `uart_dev`
branch.

## Planned Model

The project roadmap describes a logical DMA request:

```cpp
dma::Request<
    USART1,
    dma::Rx
>
```

and a separate transfer configuration:

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

These names describe the planned API rather than a currently supported
application interface.

## Current Status

DMA should be considered **roadmap / development work**.
