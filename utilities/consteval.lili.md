# Consteval and Clang

As of 2023, `clang` version 15+ still have only partial support for
`consteval`, i.e. immediate functions. To avoid compilation issues while still
allowing us to use `consteval` where it is supported, we define a macro that is
used throughout the repository in place of `consteval` directly.

```cpp
// @#'consteval.hpp'
#ifdef __clang__
#define _consteval constexpr
#else
#define _consteval consteval
#endif
// @/
