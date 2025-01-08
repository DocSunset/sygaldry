/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "Wire.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

namespace
{
    static uint8_t _tx_address = 0;
    static uint8_t _tx_idx = 0;
    static uint8_t _tx_buffer[16] = {0};
    static uint8_t _rx_idx = 0;
    static uint8_t _rx_length = 0;
    static uint8_t _rx_buffer[BUFFER_LENGTH] = {0};
    static bool _repeated_start = false;
    static constexpr unsigned int _timeout = 20*1000;
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
    printf("Wire: begin(%d, %d, %ld)\n", sda_pin, scl_pin, frequency);
    i2c_init(&i2c0_inst, frequency);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
}

void TwoWire::beginTransmission(uint8_t address)
{
    //printf("debug beginTransmission %x\n", address);
    _tx_address = address;
}

void TwoWire::write(uint8_t b)
{
    //printf("debug write %x\n", b);
    _tx_buffer[_tx_idx++] = b;
}

void TwoWire::write(uint8_t * buffer, uint8_t length)
{
    //printf("debug write "); for (int i = 0; i < length; ++i) printf("%x ", buffer[i]); printf("\n");
    for (std::size_t i = 0; i < length; ++i) write(buffer[i]);
}

void TwoWire::endTransmission(bool sendStop)
{
    //printf("debug endTransmission %d\n", sendStop);
    int bytes_written = i2c_write_timeout_us(&i2c0_inst, _tx_address, _tx_buffer, _tx_idx, not sendStop, _timeout);
    switch (bytes_written)
    {
    case PICO_ERROR_GENERIC:
        printf("TwoWire::endTransmission: failure; no subnode ACK\n");
        break;
    case PICO_ERROR_TIMEOUT:
        printf("TwoWire::endTransmission: failure; timeout\n");
        break;
    }
    _tx_idx = 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t length)
{
    //printf("debug requestFrom %x %d %d\n", address, length, _repeated_start);
    int bytes_read = i2c_read_timeout_us(&i2c0_inst, address, _rx_buffer, length, false, _timeout);
    switch(bytes_read)
    {
    case PICO_ERROR_GENERIC:
        printf("TwoWire::requestFrom: failure; no subnode ACK\n");
        return 0;
    case PICO_ERROR_TIMEOUT:
        printf("TwoWire::requestFrom: i2c bus timeout\n");
        return 0;
    default:
        _rx_idx = 0;
        _rx_length = bytes_read;
        return bytes_read;
    }
}

uint8_t TwoWire::available()
{
    //printf("debug available %d\n", _rx_length - _rx_idx);
    return _rx_length - _rx_idx;
}

uint8_t TwoWire::read()
{
    if (_rx_idx < _rx_length)
    {
        auto ret = _rx_buffer[_rx_idx++];
        //printf("debug read %d\n", ret);
        return ret;
    }
    else return 0;
}

TwoWire Wire{};