add_executable(output-logger-tests tests.cpp)
target_link_libraries(output-logger-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(output-logger-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(output-logger-tests)
