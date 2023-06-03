add_executable(string-literal-tests tests.cpp)
target_link_libraries(string-literal-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(string-literal-tests)
