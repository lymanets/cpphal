# SPI

SPI configuration exists in the current development branch, but SPI is not
documented as a completed peripheral in the current STM32F1 example set.

## Configuration Model

The current API uses options for:

```text
Baud
DataBits
Mode
Clock polarity
Clock phase
Bit order
Direction
```

A configuration is conceptually:

```cpp
hal::spi::Basic<
    /* SPI options */
>
```

## Direction

The intended modes are:

```text
FullDuplex
TxOnly
RxOnly
```

## Current Status

Treat SPI as **in development**. Do not use this page as evidence that every
listed SPI operation is currently implemented or hardware-tested.
