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
