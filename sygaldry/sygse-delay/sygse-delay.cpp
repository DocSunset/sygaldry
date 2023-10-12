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
///\defgroup sygse-delay sygse-delay: Sygaldry Delay for ESP32
///\{

void delay(unsigned long ms)
{
    ::delay(ms);
}

///\}
///\}
} }
