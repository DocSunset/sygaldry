#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <string>
#include <sstream>

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-test_reader sygbp-test_reader: Test Reader
///\{

struct TestReader
{
    std::stringstream ss;
    bool ready() {return std::stringstream::traits_type::not_eof(ss.peek());}
    char getchar() {return static_cast<char>(ss.get());}
};

///\}
///\}
} }
