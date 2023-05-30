add_executable(one-bit-bongo-first-pd-port tests.cpp)
target_link_libraries(one-bit-bongo-first-pd-port PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(one-bit-bongo-first-pd-port)
