#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygbp-cli.hpp"
#include "sygup-cstdio_logger.hpp"
#include "sygbp-cstdio_reader.hpp"

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp sygbp: Portable Bindings
///\{
///\defgroup sygbp-cstdio_cli sygbp-cstdio_cli: C Standard IO CLI Binding
///\{

/// CLI binding using the C standard input/output API to read serial data
/// \tparam Components the assembly to bind to the CLI
template<typename Components>
using CstdioCli = Cli<CstdioReader, sygup::CstdioLogger, Components>;

///\}
///\}
} }
