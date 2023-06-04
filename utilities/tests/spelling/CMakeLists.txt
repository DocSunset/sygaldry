add_executable(spelling-tests tests.cpp)
target_link_libraries(spelling-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(spelling-tests)
