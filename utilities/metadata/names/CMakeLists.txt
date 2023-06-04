add_executable(names-tests tests.cpp)
target_link_libraries(names-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(names-tests)
