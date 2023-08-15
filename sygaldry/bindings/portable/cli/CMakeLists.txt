set(lib sygaldry-bindings-portable-cli)
add_library(${lib} INTERFACE)
    target_include_directories(${lib}
        INTERFACE .
        INTERFACE ./commands
        )
    target_link_libraries(${lib}
        INTERFACE Boost::pfr
        INTERFACE sygaldry-utilities-consteval
        INTERFACE sygaldry-concepts-endpoints
        INTERFACE sygaldry-concepts-components
        INTERFACE sygaldry-concepts-metadata
        INTERFACE sygaldry-helpers-metadata
        INTERFACE sygaldry-bindings-portable-cstdio_reader
        INTERFACE sygaldry-utility_components-portable-cstdio_logger
        INTERFACE sygaldry-bindings-portable-osc_match_pattern
        INTERFACE sygaldry-bindings-portable-osc_string_constants
        )
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test
        PRIVATE ${lib}
        PRIVATE sygaldry-bindings-portable-test_component
        PRIVATE sygaldry-utility_components-portable-test_logger
        PRIVATE sygaldry-bindings-portable-test_reader
        )
catch_discover_tests(${lib}-test)
endif()
