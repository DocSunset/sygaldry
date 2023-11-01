\page page-sygaldry-docs-implementation Implementation Documentation

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The source files are annotated with literate prose meant to facilitate the
reader's understanding of the code. For more detailed documentation of the
library than the reference documentation provides, the literate sources are
the authoritative source on the behavior of the library.

See also \ref page-docs-contributing and \ref page-docs-making_a_new_component
for information on the coding conventions adopted throughout the project.

This document is partially automatically generated. See \ref page-sh-generate_implementation_index.

## Instruments
- \subpage page-sygin-t_stick

## Sensors

### Portable (sygsp)
- \subpage page-sygsp-arduino_hack
- \subpage page-sygsp-mimu_units
- \subpage page-sygsp-delay
- \subpage page-sygsp-button
- \subpage page-sygsp-micros
- \subpage page-sygsp-icm20948
- \subpage page-sygsp-complementary_mimu_fusion

### Arduino (sygsa)
- \subpage page-sygsa-trill_craft
- \subpage page-sygsa-two_wire

### ESP-IDF (sygse)
- \subpage page-sygse-button
- \subpage page-sygse-adc
- \subpage page-sygse-trill
- \subpage page-sygse-delay
- \subpage page-sygse-arduino_hack
- \subpage page-sygse-gpio
- \subpage page-sygse-micros

### Raspberry Pi Pico SDK (sygsr)
- \subpage page-sygsr-adc
- \subpage page-sygsr-button

## Utility Components (sygup)
- \subpage page-sygup-basic_logger
- \subpage page-sygup-debug_printer
- \subpage page-sygup-test_logger
- \subpage page-sygup-cstdio_logger

## Bindings

### Portable (sygbp)
- \subpage page-sygbp-cstdio_reader
- \subpage page-sygbp-test_component
- \subpage page-sygbp-output_logger
- \subpage page-sygbp-session_data
- \subpage page-sygbp-osc_string_constants
- \subpage page-sygbp-liblo
- \subpage page-sygbp-rapid_json
- \subpage page-sygbp-test_reader
- \subpage page-sygbp-spelling
- \subpage page-sygbp-cli
- \subpage page-sygbp-osc_match_pattern

### ESP-IDF (sygbe)
- \subpage page-sygbe-runtime
- \subpage page-sygbe-spiffs
- \subpage page-sygbe-libmapper_arduino
- \subpage page-sygbe-wifi

### Raspberry Pi Pico SDK (sygbr)
- \subpage page-sygbr-runtime
- \subpage page-sygbr-cli

## Helpers (sygah)
- \subpage page-sygah-mimu
- \subpage page-sygah-string_literal
- \subpage page-sygah-consteval
- \subpage page-sygah-endpoints
- \subpage page-sygah-metadata

## Concepts (sygac)
- \subpage page-sygac-runtime
- \subpage page-sygac-components
- \subpage page-sygac-endpoints
- \subpage page-sygac-mimu
- \subpage page-sygac-functions
- \subpage page-sygac-metadata

## Infrastructure
- \subpage page-docs-build_system
