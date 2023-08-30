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
unsigned long micros(void);
void delay(unsigned long ms);
//void delayMicroseconds(unsigned int us);
