\page page-sygah-consteval sygah-consteval: consteval Workaround

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

As of 2023, `clang` version 15+ still has only partial support for
`consteval`, i.e. immediate functions. To avoid compilation issues while still
allowing us to use `consteval` where it is supported, we define a macro that is
used throughout the repository in place of `consteval` directly.

```cpp
// @#'sygah-consteval.hpp'
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
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygah-consteval)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
# @/
```
