add_executable(metadata-tests tests.cpp)
target_link_libraries(metadata-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(metadata-tests)
