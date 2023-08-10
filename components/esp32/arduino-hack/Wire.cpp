/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "Wire.h"
#include "driver/i2c.h"
#include <cstdio>

namespace
{
    static constexpr i2c_port_t _port = I2C_NUM_0;
    static uint8_t _rx_idx = 0;
    static uint8_t _rx_length = 0;
    static uint8_t _rx_buffer[BUFFER_LENGTH] = {0};
    static i2c_cmd_handle_t _cmd = nullptr;
    static constexpr TickType_t _timeout = pdMS_TO_TICKS(100);

    bool cmd_link_error_check(esp_err_t err, const char * context)
    {
        switch (err)
        {
            case ESP_OK: return false;
            case ESP_ERR_INVALID_ARG:
                printf("%s: invalid argument\n", context);
                break;
            case ESP_ERR_NO_MEM:
                printf("%s: not enough memory in static buffer\n", context);
                break;
            case ESP_FAIL:
                printf("%s: not enough memory on heap\n", context);
                break;
        }
        return true;
    }

    void start_cmd_list(auto address, auto readwrite)
    {
        _cmd = i2c_cmd_link_create();
        if (_cmd == nullptr)
        {
            printf("TwoWire::start_cmd_list: unable to allocated command link\n");
            return;
        }
        auto err = i2c_master_start(_cmd);
        if (cmd_link_error_check(err, "TwoWire::start_cmd_list start bit")) return;
        err = i2c_master_write_byte(_cmd, address << 1 | readwrite, true);
        if (cmd_link_error_check(err, "TwoWire::start_cmd_list write address")) return;
    }


    void send_cmd_list()
    {
        esp_err_t err = i2c_master_cmd_begin(_port, _cmd, _timeout);
        _rx_idx = 0;
        switch (err)
        {
            case ESP_OK:
                break;
            case ESP_ERR_INVALID_ARG:
                printf("TwoWire::send_cmd_list: invalid argument\n");
                break;
            case ESP_FAIL:
                printf("TwoWire::send_cmd_list: failure; no subnode NACK\n");
                break;
            case ESP_ERR_INVALID_STATE:
                printf("TwoWire::send_cmd_list: invalid state; was TwoWire::begin() called successfully?\n");
                break;
            case ESP_ERR_TIMEOUT:
                printf("TwoWire::send_cmd_list: i2c bus timeout\n");
                break;
        }
        i2c_cmd_link_delete(_cmd);
        _cmd = nullptr;
    }
}

TwoWire::TwoWire()
{
};

void TwoWire::begin()
{
    // there's no way without arduino to know what pins to use...
    // so the user should really call begin() manually before calling
    // Trill::begin() or anything else that calls Wire::begin()...
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
    if (_cmd != nullptr)
    {
        // a command is already ongoing
        this->write(address << 1 | I2C_MASTER_WRITE);
        return;
    }
    start_cmd_list(address, I2C_MASTER_WRITE);
    _rx_length = 0;
}

void TwoWire::write(uint8_t b)
{
    auto err = i2c_master_write_byte(_cmd, b, true);
    cmd_link_error_check(err, "TwoWire::write(byte)");
}

void TwoWire::write(uint8_t * buffer, uint8_t length)
{
    auto err = i2c_master_write(_cmd, buffer, length, true);
    cmd_link_error_check(err, "TwoWire::write(buffer, length)");
}

void TwoWire::endTransmission(bool sendStop)
{
    if (not sendStop)
    {
        auto err = i2c_master_start(_cmd);
        cmd_link_error_check(err, "TwoWire::endTransmission repeated start");
        return;
    }
    auto err = i2c_master_stop(_cmd);
    printf("endTransmission\n");
    if (cmd_link_error_check(err, "TwoWire::endTransmission stop")) return;
    else send_cmd_list();
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t length)
{
    printf("requestFrom %d %d\n", address, length);
    if (_cmd == nullptr)
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
    else this->write(address << 1 | I2C_MASTER_READ);

    if (_rx_length + length > BUFFER_LENGTH)
    {
        printf("TwoWire::requestFrom: not enough room in buffer\n");
        return 0;
    }

    auto err = i2c_master_read(_cmd, _rx_buffer + _rx_length, length, I2C_MASTER_ACK);
    if (cmd_link_error_check(err, "TwoWire::requestFrom")) return 0;
    _rx_length += length;

    err = i2c_master_stop(_cmd);
    if (cmd_link_error_check(err, "TwoWire::requestFrom stop")) return 0;
    send_cmd_list();
    return _rx_length;
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
