/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

/*! \addtogroup sygah
 */
/// \{

/*! \defgroup sygah-consteval sygah-consteval: consteval Workaround

A workaround macro for spotty support of `consteval` by some compilers. Used to define the endpoint helpers.
 */
/// \{
#ifdef __clang__
#define _consteval constexpr
#else
#define _consteval consteval
#endif
/// \}
/// \}
