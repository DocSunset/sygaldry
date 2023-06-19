add_definitions(-Wfatal-errors)
add_executable(components-concepts-tests tests.cpp)
target_link_libraries(components-concepts-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(components-concepts-tests)
