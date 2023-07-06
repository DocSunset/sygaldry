add_executable(rapidjson-tests tests.cpp)
target_link_libraries(rapidjson-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(rapidjson-tests)
