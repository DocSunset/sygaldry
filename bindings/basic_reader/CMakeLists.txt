add_executable(basic-reader-tests tests.cpp)
target_link_libraries(basic-reader-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(basic-reader-tests)
