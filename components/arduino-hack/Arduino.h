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
