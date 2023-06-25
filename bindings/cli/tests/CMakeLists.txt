add_executable(cli-tests tests.cpp)
target_link_libraries(cli-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(cli-tests)

add_executable(cli-demo cli_app_demo.cpp)
