add_executable(spelling-tests tests.cpp)
target_link_libraries(spelling-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(spelling-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(spelling-tests)
