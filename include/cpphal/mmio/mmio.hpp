#pragma once
#include "access.hpp"
#include "enum_field.hpp"
#include "field.hpp"
#include "peripheral.hpp"
#include "register.hpp"
#include "register_array.hpp"

#include "device.hpp"

#define PinDef(PORT, PIN, CFG) hal::gpio::PinDef<hal::gpio::port::PORT, PIN, hal::gpio::config::CFG>

#define GetPin(CONFIGURATOR, PORT, PIN) CONFIGURATOR::GetPin<hal::gpio::port::PORT, PIN>

