# Tests

As much of the functionality of `sygaldry` is platform independent, many tests
are spread out and located near the units that they test. These are tested
using Catch2. However, tests for specific hardware platforms use the
platform-specific build toolchain and testing libraries where necessary, and
those are collected here in hopes that we can avoid having to compile, flash,
and run multiple test firmwares for one platform.
