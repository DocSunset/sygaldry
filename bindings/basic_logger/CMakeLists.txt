add_executable(basic-logger-tests tests.cpp)
target_link_libraries(basic-logger-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(basic-logger-tests)
