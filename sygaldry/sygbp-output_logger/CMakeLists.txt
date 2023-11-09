set(lib sygbp-output_logger)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygbp-spelling
        INTERFACE sygac-metadata
        INTERFACE sygac-components
        INTERFACE sygah-metadata
        INTERFACE sygbp-osc_string_constants
        INTERFACE sygup-cstdio_logger
        )

if(SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE sygac-components
        PRIVATE sygbp-test_component
        PRIVATE sygup-test_logger
        PRIVATE sygbp-output_logger
        )
catch_discover_tests(${lib}-test)
endif()
