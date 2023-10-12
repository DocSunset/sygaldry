#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/


#include <stdio.h>
#include <stdlib.h>

namespace sygaldry { namespace sygbp {
///\addtogroup sygbp
///\{
///\defgroup sygbp-cstdio_reader sygbp-cstdio_reader: C Standard Input/Output Reader
///\{

struct CstdioReader
{
    int last_read;
    bool ready()
    {
        last_read = getc(stdin);
        return last_read != EOF;
    }

    char getchar()
    {
        return last_read;
    }
};

///\}
///\}
} }
