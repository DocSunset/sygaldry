add_executable(runtime-tests tests.cpp)
target_link_libraries(runtime-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(runtime-tests)
