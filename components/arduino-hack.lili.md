# Arduino Hack

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later

TODO: this file needs to be broken down a bit.

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
the latter being a legacy include path. The header below provides forward
declarations for all of the standard core Arduino API methods. Platforms
should implement these functions as required by components that they provide.
Attempting to call an undefined method will most likely result in a linker
error, which should be taken as a signal that a new method needs to be
implemented for that platform's Arduino hack.

Several API methods that are normal provided as preprocessor macros have been
judiciously converted to static `constexpr` variables or template functions. It
is anticipated that this should not cause any incompatibilities, whereas
preprocessor macros are known to frequently cause issues.

```cpp
// @#'arduino-hack/Arduino.h'
#pragma once
/*
Copyright 2005-2013 Arduino Team.  All right reserved.
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <cstdint>

void yield();

static constexpr int HIGH = 0x1;
static constexpr int LOW =  0x0;

static constexpr int INPUT = 0x0;
static constexpr int OUTPUT = 0x1;
static constexpr int INPUT_PULLUP = 0x2;

static constexpr double PI = 3.1415926535897932384626433832795;
static constexpr double HALF_PI = 1.5707963267948966192313216916398;
static constexpr double TWO_PI = 6.283185307179586476925286766559;
static constexpr double DEG_TO_RAD = 0.017453292519943295769236907684886;
static constexpr double RAD_TO_DEG = 57.295779513082320876798154814105;
static constexpr double EULER = 2.718281828459045235360287471352;

static constexpr int SERIAL =  0x0;
static constexpr int DISPLAY = 0x1;

static constexpr int LSBFIRST = 0;
static constexpr int MSBFIRST = 1;

static constexpr int CHANGE = 1;
static constexpr int FALLING = 2;
static constexpr int RISING = 3;

constexpr auto min(const auto a, const auto b) {return (a)<(b)?(a):(b);}
constexpr auto max(const auto a, const auto b) {return (a)>(b)?(a):(b);}
constexpr auto abs(const auto x) {return (x)>0?(x):-(x);}
constexpr auto constrain(const auto amt, const auto low, const auto high) {return (amt)<(low)?(low):((amt)>(high)?(high):(amt));}
constexpr auto round(const auto x)     {return (x)>=0?(long)((x)+0.5):(long)((x)-0.5);}
constexpr auto radians(const auto deg) {return (deg)*DEG_TO_RAD;}
constexpr auto degrees(const auto rad) {return (rad)*RAD_TO_DEG;}
constexpr auto sq(const auto x) {return (x)*(x);}

void interrupts();
void noInterrupts();

//#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
//#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
//#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

constexpr uint8_t lowByte(const uint16_t w) {return (uint8_t) ((w) & 0xff);}
constexpr uint8_t highByte(const uint16_t w) {(uint8_t) ((w) >> 8);}

constexpr auto bitRead(const auto value, const auto bit) {return ((value) >> (bit)) & 0x01;}
constexpr auto bitSet(const auto value, const auto bit) {return (value) |= (1UL << (bit));}
constexpr auto bitClear(const auto value, const auto bit) {return (value) &= ~(1UL << (bit));}
constexpr auto bitToggle(const auto value, const auto bit) {return (value) ^= (1UL << (bit));}
constexpr auto bitWrite(const auto value, const auto bit, const auto bitvalue) {return (bitvalue) ? bitSet(value, bit) : bitClear(value, bit);}

// not sure that we need this...
// avr-libc defines _NOP() since 1.6.2
//#ifndef _NOP
//#define _NOP() do { __asm__ volatile ("nop"); } while (0)
//#endif

typedef unsigned int word;

constexpr unsigned long bit(auto b) {return 1UL << (b);}

typedef bool boolean;
typedef uint8_t byte;

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);
void analogWrite(uint8_t pin, int val);

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);void delay(unsigned long ms);

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
void detachInterrupt(uint8_t interruptNum);

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
void noTone(uint8_t _pin);

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned long);
long map(long, long, long, long, long);
// @/

// @#'arduino-hack/WProgram.h'
#include "Arduino.h"
// @/
```

# WCharacter.h

This header is actually fully portable C++, so we copy it directly from Arduino here:

```cpp
// @#'arduino-hack/WCharacter.h'
/*
 WCharacter.h - Character utility functions for Wiring & Arduino
 Copyright (c) 2010 Hernando Barragan.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Character_h
#define Character_h

#include <ctype.h>

// WCharacter.h prototypes
inline boolean isAlphaNumeric(int c) __attribute__((always_inline));
inline boolean isAlpha(int c) __attribute__((always_inline));
inline boolean isAscii(int c) __attribute__((always_inline));
inline boolean isWhitespace(int c) __attribute__((always_inline));
inline boolean isControl(int c) __attribute__((always_inline));
inline boolean isDigit(int c) __attribute__((always_inline));
inline boolean isGraph(int c) __attribute__((always_inline));
inline boolean isLowerCase(int c) __attribute__((always_inline));
inline boolean isPrintable(int c) __attribute__((always_inline));
inline boolean isPunct(int c) __attribute__((always_inline));
inline boolean isSpace(int c) __attribute__((always_inline));
inline boolean isUpperCase(int c) __attribute__((always_inline));
inline boolean isHexadecimalDigit(int c) __attribute__((always_inline));
inline int toAscii(int c) __attribute__((always_inline));
inline int toLowerCase(int c) __attribute__((always_inline));
inline int toUpperCase(int c)__attribute__((always_inline));


// Checks for an alphanumeric character. 
// It is equivalent to (isalpha(c) || isdigit(c)).
inline boolean isAlphaNumeric(int c) 
{
  return ( isalnum(c) == 0 ? false : true);
}


// Checks for an alphabetic character. 
// It is equivalent to (isupper(c) || islower(c)).
inline boolean isAlpha(int c)
{
  return ( isalpha(c) == 0 ? false : true);
}


// Checks whether c is a 7-bit unsigned char value 
// that fits into the ASCII character set.
inline boolean isAscii(int c)
{
  return ( isascii (c) == 0 ? false : true);
}


// Checks for a blank character, that is, a space or a tab.
inline boolean isWhitespace(int c)
{
  return ( isblank (c) == 0 ? false : true);
}


// Checks for a control character.
inline boolean isControl(int c)
{
  return ( iscntrl (c) == 0 ? false : true);
}


// Checks for a digit (0 through 9).
inline boolean isDigit(int c)
{
  return ( isdigit (c) == 0 ? false : true);
}


// Checks for any printable character except space.
inline boolean isGraph(int c)
{
  return ( isgraph (c) == 0 ? false : true);
}


// Checks for a lower-case character.
inline boolean isLowerCase(int c)
{
  return (islower (c) == 0 ? false : true);
}


// Checks for any printable character including space.
inline boolean isPrintable(int c)
{
  return ( isprint (c) == 0 ? false : true);
}


// Checks for any printable character which is not a space 
// or an alphanumeric character.
inline boolean isPunct(int c)
{
  return ( ispunct (c) == 0 ? false : true);
}


// Checks for white-space characters. For the avr-libc library, 
// these are: space, formfeed ('\f'), newline ('\n'), carriage 
// return ('\r'), horizontal tab ('\t'), and vertical tab ('\v').
inline boolean isSpace(int c)
{
  return ( isspace (c) == 0 ? false : true);
}


// Checks for an uppercase letter.
inline boolean isUpperCase(int c)
{
  return ( isupper (c) == 0 ? false : true);
}


// Checks for a hexadecimal digits, i.e. one of 0 1 2 3 4 5 6 7 
// 8 9 a b c d e f A B C D E F.
inline boolean isHexadecimalDigit(int c)
{
  return ( isxdigit (c) == 0 ? false : true);
}


// Converts c to a 7-bit unsigned char value that fits into the 
// ASCII character set, by clearing the high-order bits.
inline int toAscii(int c)
{
  return toascii (c);
}


// Warning:
// Many people will be unhappy if you use this function. 
// This function will convert accented letters into random 
// characters.

// Converts the letter c to lower case, if possible.
inline int toLowerCase(int c)
{
  return tolower (c);
}


// Converts the letter c to upper case, if possible.
inline int toUpperCase(int c)
{
  return toupper (c);
}

#endif
// @/
```

# Arduino Libraries

The library headers below are meant to be included by platform specific
Arduino hack subsystems that will implement the Arduino API for a given
platform. It's likely that certain APIs provided by official Arduino
libraries are missing below; if this is the case, they should be added
as required.

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
    void endTransmission(bool sendStop);
    void endTransmission();
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
