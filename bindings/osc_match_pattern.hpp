#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// TODO: this should probably not be a header only implementation
/// Match an OSC address pattern against the given address
bool osc_match_pattern(const char * pattern, const char * address)
{
    switch (*pattern)
    {
    case '*':
        break;
    case '?':
        break;
    case '[':
        break;
    case '{':
        break;
    case '\0':
        if (*address == '\0') return true;
        break;
    default:
        if (*pattern == *address) return osc_match_pattern(++pattern, ++address);
        break;
    }
    return false;
}
