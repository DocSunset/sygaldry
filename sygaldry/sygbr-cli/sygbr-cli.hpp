#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include "sygbp-cli.hpp"
#include "sygup-cstdio_logger.hpp"

namespace sygaldry { namespace sygbr {

/// \addtogroup sygbr
/// \{

/// \defgroup sygbr-cli sygbr-cli: Raspberyy Pi Pico SDK CLI
/// Literate source code: page-sygbr-cli
/// \{

/// Command line input reader for Pico SDK
/*!
Remember that `ready()` must be called before each call to `getchar()`,
which merely returns the last character received in the check by `ready()`
*/
struct PicoReader
{
    int last_read;
    bool ready()
    {
        last_read = getchar_timeout_us(0);
        return last_read != PICO_ERROR_TIMEOUT;
    }

    char getchar()
    {
        return last_read;
    }
};

/// CLI binding using the C standard input/output API to read serial data
/// \tparam Components the assembly to bind to the CLI
template<typename Components>
using PicoCli = sygbp::Cli<PicoReader, sygup::CstdioLogger, Components>;

/// \}
/// \}

} }
