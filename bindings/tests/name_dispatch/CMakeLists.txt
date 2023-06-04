add_executable(dispatch-tests tests.cpp)
target_link_libraries(dispatch-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(dispatch-tests)
