/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include <sstream>
#include "sygaldry-utility_components-portable-basic_logger.hpp"

namespace sygaldry { namespace utility_components {

struct TestPutter
{
    std::stringstream ss;
    void operator()(char c)
    {
        ss << c;
    }
};

using TestLogger = BasicLogger<TestPutter>;

} }
