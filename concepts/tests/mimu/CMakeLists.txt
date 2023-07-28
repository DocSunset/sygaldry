add_executable(mimu-concepts-tests tests.cpp)
target_link_libraries(mimu-concepts-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mimu-concepts-tests)
