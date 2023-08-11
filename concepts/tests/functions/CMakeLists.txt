add_executable(functions-concepts-tests tests.cpp)
target_link_libraries(functions-concepts-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(functions-concepts-tests PRIVATE Sygaldry::Concepts)
catch_discover_tests(functions-concepts-tests)
