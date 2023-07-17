# ESP32 Arduino Hack

This document describes the ESP32 implementation of the Arduino APIs supported
by Sygaldry's Arduino hack subsystem. Although an Arduino library implementation
exists for ESP32, it tends to lag behind the main ESP-IDF version. At the time
of writing, the ESP32 Arduino library is not available with a compiler that is
compatible with Sygaldry, so Arduino hack is necessary.

An IDF component `CMakeLists.txt` registers
the implementation with the framework, and an implementation of the `TwoWire`
API is provided.

# ESP Component CMakeLists.txt

```cmake
# @#'arduino-hack/CMakeLists.txt'
idf_component_register( SRC_DIRS ${SYGALDRY_ROOT}/components/esp32/arduino-hack
                        INCLUDE_DIRS ${SYGALDRY_ROOT}/components/arduino-hack
                        REQUIRES driver
                      )
# @/
```

# ESP Arduino.h

```cpp
// @#'arduino-hack/Arduino.cpp'
#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void delay(unsigned long ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}
// @/
```

# ESP TwoWire API

We define a very simple implementation of this API in terms of the ESP-IDF.
It remains as future work to properly document this implementation, but it
is hoped that the reader will find it reasonably straightforward.

```cpp
// @#'arduino-hack/Wire.cpp'
#include "Wire.h"
#include "driver/i2c.h"
#include <cstdio>

namespace
{
	static constexpr i2c_port_t _port = I2C_NUM_0;
	static uint8_t _tx_address = 0;
	static uint8_t _tx_idx = 0;
	static uint8_t _tx_buffer[4] = {0};
	static uint8_t _rx_idx = 0;
	static uint8_t _rx_length = 0;
	static uint8_t _rx_buffer[BUFFER_LENGTH] = {0};
	static constexpr TickType_t _timeout = pdMS_TO_TICKS(100);
}

TwoWire::TwoWire()
{
};

void TwoWire::begin()
{
	// there's no way without arduino to know what pins to use...
	// so the user should really call begin() manually before calling Trill::begin()...
}

void TwoWire::begin(int sda_pin, int scl_pin, uint32_t frequency)
{
	if (frequency > 4000000) frequency = 4000000;
	i2c_config_t config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda_pin,
		.scl_io_num = scl_pin,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master = { .clk_speed = frequency, },
		.clk_flags = 0,
	};

	esp_err_t err = i2c_param_config(_port, &config);
	if (err != ESP_OK)
	{
		printf("Wire error: i2c_param_config invalid argument\n");
		return;
	}

	err = i2c_driver_install(_port, config.mode, 0, 0, 0);
	if (err == ESP_OK) return;
	switch (err)
	{
		case ESP_ERR_INVALID_ARG:
			printf("Wire error: i2c_driver_install invalid argument\n");
			return;
		case ESP_FAIL:
			printf("Wire error: i2c_driver_install failure\n");
			return;
	}
}

void TwoWire::beginTransmission(uint8_t address)
{
	_tx_address = address;
}

void TwoWire::write(uint8_t b)
{
	_tx_buffer[_tx_idx++] = b;
}

void TwoWire::endTransmission()
{
	esp_err_t err = i2c_master_write_to_device(_port, _tx_address, _tx_buffer, _tx_idx, _timeout);
	_tx_idx = 0;
	if (err == ESP_OK) return;
	switch (err)
	{
		case ESP_ERR_INVALID_ARG:
			printf("TwoWire::endTransmission: invalid argument\n");
			return;
		case ESP_FAIL:
			printf("TwoWire::endTransmission: failure; no subnode NACK\n");
			return;
		case ESP_ERR_INVALID_STATE:
			printf("TwoWire::endTransmission: invalid state; was TwoWire::begin() called successfully?\n");
			return;
		case ESP_ERR_TIMEOUT:
			printf("TwoWire::endTransmission: i2c bus timeout\n");
			return;
	}
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t length)
{
	esp_err_t err = i2c_master_read_from_device(_port, address, _rx_buffer, length, _timeout);
	_rx_idx = 0;
	switch(err)
	{
		case ESP_OK:
			_rx_length = length;
			return length;
		case ESP_ERR_INVALID_ARG:
			printf("TwoWire::requestFrom: invalid argument\n");
			break;
		case ESP_FAIL:
			printf("TwoWire::requestFrom: failure; no subnode NACK\n");
			break;
		case ESP_ERR_INVALID_STATE:
			printf("TwoWire::requestFrom: invalid state; was TwoWire::begin() called successfully?\n");
			break;
		case ESP_ERR_TIMEOUT:
			printf("TwoWire::requestFrom: i2c bus timeout\n");
			break;
	}
	return 0;
}

uint8_t TwoWire::available()
{
	return _rx_length - _rx_idx;
}

uint8_t TwoWire::read()
{
	if (_rx_idx < _rx_length)
		return _rx_buffer[_rx_idx++];
	else return 0;
}

TwoWire Wire{};
// @/
```
