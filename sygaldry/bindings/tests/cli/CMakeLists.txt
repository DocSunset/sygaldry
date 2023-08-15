add_executable(cli-tests tests.cpp)
target_link_libraries(cli-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(cli-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(cli-tests)
