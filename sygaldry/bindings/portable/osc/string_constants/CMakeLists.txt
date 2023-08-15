set(lib sygaldry-bindings-portable-osc_string_constants)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
    target_link_libraries(${lib}
            INTERFACE sygaldry-concepts-components
            INTERFACE sygaldry-concepts-endpoints
            INTERFACE sygaldry-bindings-portable-spelling
            )
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test
        PRIVATE sygaldry-utilities-consteval
        PRIVATE sygaldry-helpers-endpoints
        PRIVATE ${lib}
        )
catch_discover_tests(${lib}-test)
endif()
