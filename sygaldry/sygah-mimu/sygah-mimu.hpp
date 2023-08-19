#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry {
///\defgroup helpers_mimu_endpoints MIMU Endpoints Helpers
///\{
/*! \brief A MIMU data vector
\tparam name The name of the vector, e.g. "accelerometer"
\tparam T The underlying numeric type, usually float or int
\tparam min The expected minimum magnitude of the vector
\tparam max The expected maximum magnitude of the vector
\tparam unit The unit of measurement of the vector
\tparam desc A textual description of the vector
\tparam Tags A list of tag helper classes to apply to the endpoint
*/
template< string_literal name
        , typename T = float
        , num_literal<T> min = -1.0f
        , num_literal<T> max = 1.0f
        , string_literal unit = "normalized"
        , string_literal desc = ""
        , typename ... Tags
        >
struct vec3_message
: array_message<name, 3, desc, T, min, max, T{}, Tags...>
, unit_<unit>
{
    /// The parent type; this is defined as a convenience for implementation and shouldn't be treated as part of the public API
    using Parent = array_message<name, 3, desc, T, min, max, T{}, Tags...>;
    using Parent::operator=;
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& x() noexcept { return Parent::state[0]; }
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& y() noexcept { return Parent::state[1]; }
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& z() noexcept { return Parent::state[2]; }
    /// Immutable vector component access
    constexpr const auto& x() const noexcept { return Parent::state[0]; }
    /// Immutable vector component access
    constexpr const auto& y() const noexcept { return Parent::state[1]; }
    /// Immutable vector component access
    constexpr const auto& z() const noexcept { return Parent::state[2]; }
};
///\}
}
