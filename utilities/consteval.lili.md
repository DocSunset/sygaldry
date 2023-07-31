\page consteval_and_clang Consteval and Clang

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

As of 2023, `clang` version 15+ still have only partial support for
`consteval`, i.e. immediate functions. To avoid compilation issues while still
allowing us to use `consteval` where it is supported, we define a macro that is
used throughout the repository in place of `consteval` directly.

```cpp
// @#'consteval.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#ifdef __clang__
#define _consteval constexpr
#else
#define _consteval consteval
#endif
// @/
