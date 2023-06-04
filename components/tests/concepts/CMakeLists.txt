add_executable(concepts-tests tests.cpp)
target_link_libraries(concepts-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(concepts-tests)
