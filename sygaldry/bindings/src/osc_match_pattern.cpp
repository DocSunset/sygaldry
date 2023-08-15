/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "bindings/osc_match_pattern.hpp"

/// Match an OSC address pattern against the given address
bool osc_match_pattern(const char * pattern, const char * address)
{
    switch (*pattern)
    {
    case '*':
        while (*pattern == '*') ++pattern;
        while (*address && *address != '/' && *address != *pattern) ++address;
        return osc_match_pattern(pattern, address);
    case '[':
    {
        bool negate = *++pattern == '!';
        bool match = false;
        while (not match)
        {
            if (*pattern == '\0') return false;
            if (*pattern == ']') break;
            if (*(pattern + 1) == '-' && (*(pattern + 2) && *(pattern + 2) != ']'))
            {
                char start = *pattern;
                char end = *(pattern + 2);
                pattern = pattern + 3;
                if (start < end) match = start <= *address && *address <= end;
                else             match = end   <= *address && *address <= start;
            }
            else if (*pattern++ == *address) match = true;
        }
        while (*pattern && *pattern != ']') ++pattern;
        if (*pattern == '\0') return false;
        if (negate) match = not match;
        if (match) return osc_match_pattern(++pattern, ++address);
        else return false;
        break;
    }
    case '{':
    {
        ++pattern;
        const char * address_start = address;
        bool match = false;
        while (not match)
        {
            if (*pattern == '\0') return false;
            if (*pattern == '}') break;
            if (*pattern++ == *address++)
            {
                if (*pattern == ',' || *pattern == '}') match = true;
            }
            else
            {
                address = address_start;
                while (*pattern && *pattern != '}' && *pattern != ',') ++pattern;
                if (*pattern == ',') ++pattern;
            }
        }
        while (*pattern && *pattern != '}') ++pattern;
        if (*pattern == '\0') return false;
        if (match) return osc_match_pattern(++pattern, address);
        else return false;
        break;
    }
    case '\0':
        if (*address == '\0') return true;
        break;
    case '/':
        if (*(pattern+1) != '/') /*fallthrough*/;
        else
        {
            ++pattern;
            while (not osc_match_pattern(pattern, address))
            {
                while (*address)
                {
                    ++address;
                    if (*address == '/') break;
                }
                if (*address == '\0') return false;
            }
            return true;
        }
        [[fallthrough]];
    default:
        if (*pattern == '?' || *pattern == *address) return osc_match_pattern(++pattern, ++address);
        else return false;
        break;
    }
    return false;
}
