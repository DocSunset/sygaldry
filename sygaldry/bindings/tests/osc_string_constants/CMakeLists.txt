add_executable(osc-string-constants-tests tests.cpp)
target_link_libraries(osc-string-constants-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(osc-string-constants-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(osc-string-constants-tests)
