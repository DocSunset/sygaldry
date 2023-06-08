add_compile_options(-fconcepts-diagnostics-depth=2)
add_executable(concepts-metadata-tests tests.cpp)
target_link_libraries(concepts-metadata-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(concepts-metadata-tests)
