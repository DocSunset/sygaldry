\page comp_arduino_hack Arduino Hack

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later

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
preprocessor symbol is undefined. A cursory and incomplete investigation
suggests that `Arduino.h` and `WProgram.h` provide identical APIs, with
the latter being a legacy include path.

```cpp
// @#'arduino-hack/Arduino.h'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <cstdint>

static constexpr uint8_t HIGH = 1;
static constexpr uint8_t LOW  = 0;
static constexpr uint8_t INPUT = 0x0;
static constexpr uint8_t OUTPUT = 0x1;
static constexpr uint8_t INPUT_PULLUP = 0x2;

typedef bool boolean;

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
//int digitalRead(uint8_t pin);
//int analogRead(uint8_t pin);

//unsigned long millis(void);
//unsigned long micros(void);
void delay(unsigned long ms);
//void delayMicroseconds(unsigned int us);
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
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

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
	void write(uint8_t * buffer, uint8_t length);
	void endTransmission(bool sendStop = true);
	uint8_t requestFrom(uint8_t address, uint8_t reg);
	uint8_t available();
	uint8_t read();
};

extern TwoWire Wire;
// @/
```

# SPI.h

The `SPIClass` API provides access to the SPI bus.

```cpp
// @#'arduino-hack/SPI.h'
#pragma once
/*
Copyright 2010 by Cristian Maglie <c.maglie@@arduino.cc>
Copyright 2014 by Paul Stoffregen <paul@@pjrc.com> (Transaction API)
Copyright 2014 by Matthijs Kooijman <matthijs@@stdin.nl> (SPISettings AVR)
Copyright 2014 by Andrew J. Kroll <xxxajk@@gmail.com> (atomicity fixes)
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France (port to Sygladry Arduino Hack)

SPI Master library for arduino.

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <stdint.h>

static constexpr uint8_t LSBFIRST = 0;
static constexpr uint8_t MSBFIRST = 1;

static constexpr uint8_t SPI_MODE0 = 0;
static constexpr uint8_t SPI_MODE1 = 1;
static constexpr uint8_t SPI_MODE2 = 2;
static constexpr uint8_t SPI_MODE3 = 3;

class SPISettings
{
public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);
    SPISettings();
    uint32_t _clock;
    uint8_t  _bitOrder;
    uint8_t  _dataMode;
};

class SPIClass
{
public:
    /// Initialize the SPI library
    static void begin();

    /// Acquire the SPI bus
    /// Before using SPI.transfer() or asserting chip select pins,
    /// this function is used to gain exclusive access to the SPI bus
    /// and configure the correct settings.
    static void beginTransaction(SPISettings settings);

    /// Read and write a single byte over the SPI bus
    static uint8_t transfer(uint8_t data);

    /// Read and write two bytes over the SPI bus
    static uint16_t transfer16(uint16_t data);

    /// Read and write a buffer of `count` bytes over the SPI bus
    static void transfer(void *buf, size_t count);

    /// Release the SPI bus
    /// After performing a group of transfers and releasing the chip select
    /// signal, this function allows others to access the SPI bus
    static void endTransaction(void);

    /// Disable the SPI bus
    static void end();
};

extern SPIClass SPI;
// @/
```

