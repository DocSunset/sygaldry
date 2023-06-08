add_executable(endpoints-tests tests.cpp)
target_link_libraries(endpoints-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(endpoints-tests)
