#pragma once
/*
Copyright 2010 by Cristian Maglie <c.maglie@arduino.cc>
Copyright 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
Copyright 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
Copyright 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
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
