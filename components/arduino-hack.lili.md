# Arduino Hack

This document describes a collection of crude but effective hacks used to trick
certain third party libraries into compiling despite the absence of a full
blown Arduino environment on which they were originally intended to depend.
Currently this is only used to enable use of the `Trill-Arduino` library, and
as such there are large portions of the full Arduino API missing.

The Arduino ecosystem consists of boards made by a variety of manufacturers,
using a variety of microcontroller hardware, and API-compatible libraries for
said boards. Despite that the APIs are meant to be hardware independent, and
could in principle be developed as such (e.g. using the same header files),
most of "the Arduino Library" so to speak is duplicated for each independent
microcontroller platform.

This Arduino hack subsystem provides such hardware independent headers.
Platform-dependent implementations of these supported Arduino APIs are then
found in the related platform-specific subdirectories. For platforms based on
Arduino and using a compatible compiler, the upstream Arduino implementation
should be preferred. The Arduino hack is intended for platforms (such as ESP32,
and Raspberry Pi Pico) where first party Arduino API support is not available,
to establish the bare minimum Arduino compatible API required to make use of
pre-existing driver libraries for various sensors, such as Bela's Trill sensors.

# Arduino.h and WProgram.h

This Arduino header is included by `Trill-Arduino` when the `ARDUINO`
preprocessor symbol is undefined. A cursory an incomplete investigation
suggests that `Arduino.h` and `WProgram.h` provide identical APIs, with
the latter being a legacy include path.

```cpp
// @#'arduino-hack/Arduino.h'
#pragma once
#include <cstdint>

typedef bool boolean;

void delay(unsigned long ms);
// @/

// @#'arduino-hack/WProgram.h'
#include "Arduino.h"
// @/
```

# Wire.h

The `TwoWire` API provides access to the I2C bus.

```cpp
// @#'arduino-hack/Wire.h'
#pragma once

#include <cstdint>

#define BUFFER_LENGTH 60

class TwoWire
{
public:
	TwoWire();
	void begin();
	void begin(int sda_pin, int scl_pin, uint32_t frequency);
	void beginTransmission(uint8_t address);
	void write(uint8_t byte);
	void endTransmission();
	uint8_t requestFrom(uint8_t address, uint8_t reg);
	uint8_t available();
	uint8_t read();
};

extern TwoWire Wire;
// @/
```
