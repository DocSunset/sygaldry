add_executable(functions-concepts-tests tests.cpp)
target_link_libraries(functions-concepts-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(functions-concepts-tests)
