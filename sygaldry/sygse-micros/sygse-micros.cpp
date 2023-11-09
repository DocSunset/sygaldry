/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "Arduino.h"

namespace sygaldry { namespace sygsp {
///\addtogroup sygse
///\{
///\defgroup sygse-micros sygse-micros: Portable Timestamp API for ESP32
/// Literate source code: \ref page-sygse-micros
///\{

/// Get the number of microseconds elapsed since boot.
unsigned long micros()
{
    return ::micros();
}

///\}
///\}
} }
