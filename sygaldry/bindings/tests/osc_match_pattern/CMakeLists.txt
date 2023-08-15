add_executable(osc-match-tests tests.cpp)
target_link_libraries(osc-match-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(osc-match-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(osc-match-tests)
