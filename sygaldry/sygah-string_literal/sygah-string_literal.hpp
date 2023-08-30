#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <cstddef>
#include "sygah-consteval.hpp"

namespace sygaldry {

/// \ingroup sygah Sygaldry Helpers
/// \{

/// \defgroup sygah-string_literal String Literal Template Parameter Wrapper
/// \{

/// A wrapper around a string lteral that enables using strings as template parameters.
template<std::size_t N>
struct string_literal
{
    /// The string buffer
    char value[N];
    /// Constructor from string literal
    _consteval string_literal(const char (&str)[N]) noexcept
    {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
};

/// \}
/// \}
}
