set(lib sygaldry-bindings-portable-output_logger)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
    target_link_libraries(${lib}
            INTERFACE sygaldry-bindings-portable-spelling
            INTERFACE sygaldry-concepts-metadata
            INTERFACE sygaldry-concepts-components
            INTERFACE sygaldry-helpers-metadata
            INTERFACE sygaldry-bindings-portable-osc_string_constants
            INTERFACE sygaldry-utility_components-portable-cstdio_logger
            )
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})

if (CMAKE_BUILD_TYPE)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE sygaldry-concepts-components
        PRIVATE sygaldry-bindings-portable-test_component
        PRIVATE sygaldry-utility_components-portable-test_logger
        PRIVATE sygaldry-bindings-portable-output_logger
        )
catch_discover_tests(${lib}-test)
endif()
