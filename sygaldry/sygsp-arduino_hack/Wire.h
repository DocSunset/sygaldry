#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

/// \addtogroup sygsp-arduino_hack
/// \{

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

/// \}
