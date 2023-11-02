set(lib sygbp-cli)
add_library(${lib} INTERFACE)
target_include_directories(${lib}
    INTERFACE .
    INTERFACE ./commands
    )
target_link_libraries(${lib}
    INTERFACE Boost::pfr
    INTERFACE sygah-consteval
    INTERFACE sygac-endpoints
    INTERFACE sygac-components
    INTERFACE sygac-metadata
    INTERFACE sygah-metadata
    INTERFACE sygbp-cstdio_reader
    INTERFACE sygup-cstdio_logger
    INTERFACE sygbp-osc_match_pattern
    INTERFACE sygbp-osc_string_constants
    )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test
        PRIVATE ${lib}
        PRIVATE sygbp-test_component
        PRIVATE sygup-test_logger
        PRIVATE sygbp-test_reader
        )
catch_discover_tests(${lib}-test)
endif()
