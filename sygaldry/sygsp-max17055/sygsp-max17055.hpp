/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sygah-metadata.hpp"
#include "sygsp-max17055_tests.hpp"
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygah-endpoints.hpp"
#include "sygsp-max17055-registers.hpp"

namespace sygaldry { namespace sygsp {

template<typename Serif, typename AK09916Serif>
struct ICM20948
: name_<"MAX17055 Fuel Gauge">
{
    struct inputs_t {
        // Initialisation Elements
        uint8_t i2c_addr; // i2c address of the fuel guage
        int designcap; // design capacity of the batteries in mAh
        int ichg; // Charge termination current in mA
        int rsense; // Resistance of current sense resistor (mOhm))
        float vempty; // Empty voltage of the battery (V)
        float recovery_voltage; // Recovery voltage of the battery (V)

        // Learned Parameters
        int soc; // raw State of charge (5mVh/rsense)
        int rcomp; // compensation parameter for battery
        int tempco; // temperature compensation parameter
        int fullcap; // full capacity of battery (5mVh/rsense)
        int fullcapnorm; // full capacity of battery normalised
        int cycles; // charge cycles of the battery
    } inputs;

    struct outputs_t {

        // ANALOG MEASUREMENTS
        // Current
        range_<-32768, 32767, 0> inst_curr_raw;
        range_<-32768, 32767, 0> avg_curr_raw;
        range_<1/64000.0f, 10/64f, 1/640.0f> curr_sensitivity;
        range_<-5.12f, 5.12f, 0> inst_curr;
        range_<-5.12f, 5.12f, 0> avg_curr;
        // Voltage
        range_<0, 65535, 0> inst_voltage_raw;
        range_<0, 65535, 0> avg_voltage_raw;
        range_<0.0f, 5.11992f, 0.0f> inst_voltage;
        range_<0.0f, 5.11992f, 0.0f> avg_voltage;

        // MODEL OUTPUTS
        // Capacity
        range_<0, 65535, 0> capacity_raw; // LSB
        range_<0, 65535, 0> fullcapacity_raw; // LSB
        range_<0.005f, 5f, 0.5f> capacity_sensitivity; // mAh/LSB
        range_< 0.0f, 32000.0f, 0> capacity; //mAh
        range_< 0.0f, 32000.0f, 0> fullcapacity; // mAh
        // Capacity (norm)
        range_<0,65535,0> fullcapacitynorm_raw;
        range_<0.0f,255.9961f,0.0f> fullcapacitynorm; // percentage
        // SOC, Age
        range_<0,65535,0> soc_raw; // LSB
        range_<0,65535,0> age_raw; // LSB
        range_<0.0f,255.9961f,0.0f> soc; // percentage
        range_<0.0f,255.9961f,0.0f> age; // percentage
        // Time to full (TTF), Time to empty (TTE), age
        range_<0,65535,0> ttf_raw; // LSB
        range_<0,65535,0> tte_raw; // LSB
        range_<0.0f,102.3984f,0.0f> ttf; // hours
        range_<0.0f,102.3984f,0.0f> tte;  // hours
        // Cycles
        range_<0,65535,0> chargecyles_raw;
        range_<0.0f,655.35f,0.0f> chargecyles;
        // Parameters
        range_<0,65535,0> rcomp_out;
        range_<0,65535,0> tempcomp_out;

        // Battery Status
        bng<"present"> status;
        bng<"removed"> removed;
        bng<"inserted"> inserted;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    /// initialize the MAX17055 for continuous reading
    void init()
    {

    }

    // poll the ICM20948 for new data and update endpoints
    void main()
    {
        if (!outputs.running) return; // TODO: retry connecting every so ofte
        static auto prev = micros();
        auto now = micros();
        bool read = false;

        

        if (read)
        {
            outputs.elapsed = now - prev;
            prev = now;
        }
    }
};

} }
