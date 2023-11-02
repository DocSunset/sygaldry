set(lib sygbp-osc_string_constants)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac-components
        INTERFACE sygac-endpoints
        INTERFACE sygbp-spelling
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test
        PRIVATE sygah-consteval
        PRIVATE sygah-endpoints
        PRIVATE ${lib}
        )
catch_discover_tests(${lib}-test)
endif()
