add_executable(mimu-helper-tests tests.cpp)
target_link_libraries(mimu-helper-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mimu-helper-tests)
