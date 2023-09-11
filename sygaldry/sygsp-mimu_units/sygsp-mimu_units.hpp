/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once

namespace sygaldry { namespace sygsp {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-mimu_units MIMU Units

This software component defines several unit conversion factors.
*/
/// \{

/*! Meters per second squared per standard unit gravity

Multiply a quantity in units g by this constant to access its equivalent in m/s/s.
*/
static constexpr float mss_per_g = 9.80665; // standard gravity according to Wikipedia, citing the International Bureau of Weights and Measures

/*! Units gravity per unit meter per second squared

Multiply a quantity in units m/s/s by this constant to access its equivalent in g.
*/
static constexpr float g_per_mss = 1.0/mss_per_g;

/*! Radians per degree

Multiply a quantity in degrees by this constant to access its equivalent in radians
*/
static constexpr float rad_per_deg = std::numbers::pi / 180.0;

/*! Degrees per radian

Multiply a quantity in radians by this constant to access its equivalent in degrees
*/
static constexpr float deg_per_rad = 180.0 / std::numbers::pi;

/// \}
/// \}

} }
