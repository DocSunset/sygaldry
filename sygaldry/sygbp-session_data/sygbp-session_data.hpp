#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygac-components.hpp"
#include "sygac-endpoints.hpp"

namespace sygaldry { namespace sygbp {

template<typename C>
constexpr void for_each_session_datum(C& components, auto fn)
{
    for_each_endpoint(components, [&]<typename T>(T& endpoint)
    {
        if constexpr (tagged_session_data<T>) fn(endpoint);
    });
}

} }
