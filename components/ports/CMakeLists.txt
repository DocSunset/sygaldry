add_executable(ports-tests tests.cpp)
target_link_libraries(ports-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(ports-tests)
