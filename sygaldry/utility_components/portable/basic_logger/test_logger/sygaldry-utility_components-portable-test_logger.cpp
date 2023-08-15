/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-utility_components-portable-test_logger.hpp"
#include "sygaldry-utility_components-portable-basic_logger.impl.hpp"

namespace sygaldry { namespace utility_components {
template struct BasicLogger<TestPutter>;
std::stringstream TestPutter::ss;
} }
