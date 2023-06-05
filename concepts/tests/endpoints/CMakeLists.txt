add_compile_options(-fconcepts-diagnostics-depth=2)
add_executable(concepts-endpoints-tests tests.cpp)
target_link_libraries(concepts-endpoints-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(concepts-endpoints-tests)
