/*
*/
#pragma once
#include <lo.h>
#include <lo_serverthread.h>
#include <lo_types.h>

namespace sygaldry { namespace bindings {

template<typename Components>
struct LibloOsc
: name_<"Liblo OSC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: LGPL-2.1-or-later"
, version_<"0.0.0">
, description_<"Open Sound Control bindings using the liblo library">
{
    void init(Components& components)
    {
    }

    void main(Components& components)
    {
    }
};

} }
