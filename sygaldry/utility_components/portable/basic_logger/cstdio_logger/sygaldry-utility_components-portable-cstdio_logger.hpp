/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <cstdio>
#include <cstdlib>
#include "sygaldry-utility_components-portable-basic_logger.hpp"

namespace sygaldry { namespace utility_components {

struct CstdioPutter
{
    void operator()(char c)
    {
        auto retcode = std::putchar(c);
        if (retcode == EOF)
        {
            // I guess we're unexpectedly done now?
            std::exit(EXIT_FAILURE);
        }
    }
};

using CstdioLogger = BasicLogger<CstdioPutter>;

} }
