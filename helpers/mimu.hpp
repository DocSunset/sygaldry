#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry {
template< string_literal name
        , typename T = float
        , num_literal<T> min = -1.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = 0.0f
        , string_literal unit = "normalized"
        , string_literal desc = ""
        , typename ... Tags
        >
struct vec3_message
: array_message<name, 3, desc, T, min, max, init, Tags...>
, unit_<unit>
{
    using Parent = array_message<name, 3, desc, T, min, max, init, Tags...>;
    using Parent::operator=;
    constexpr auto& x() noexcept { return Parent::state[0]; }
    constexpr auto& y() noexcept { return Parent::state[1]; }
    constexpr auto& z() noexcept { return Parent::state[2]; }
    constexpr const auto& x() const noexcept { return Parent::state[0]; }
    constexpr const auto& y() const noexcept { return Parent::state[1]; }
    constexpr const auto& z() const noexcept { return Parent::state[2]; }
};
}
