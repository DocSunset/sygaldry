#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <cstddef>
#include "utilities/consteval.hpp"

namespace sygaldry {

/// A wrapper around a string lteral that enables using strings as template parameters.
template<std::size_t N>
struct string_literal
{
    char value[N];
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

#define metadata_struct(NAME) template<string_literal str>\
struct NAME##_\
{\
    static _consteval auto NAME() {return str.value;}\
}

metadata_struct(name);
metadata_struct(author);
metadata_struct(email);
metadata_struct(license);
metadata_struct(copyright);
metadata_struct(description);
metadata_struct(uuid);
metadata_struct(unit);
metadata_struct(version);
metadata_struct(date);

#undef metadata_struct

}
