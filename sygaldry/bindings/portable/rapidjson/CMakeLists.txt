set(lib sygaldry-bindings-portable-rapid_json)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE ${SYGALDRY_ROOT}/dependencies/rapidjson/include
            INTERFACE .
            )
    target_link_libraries(${lib}
            INTERFACE sygaldry-concepts-endpoints
            INTERFACE sygaldry-concepts-components
            INTERFACE sygaldry-helpers-metadata
            INTERFACE sygaldry-bindings-portable-osc_string_constants
            INTERFACE sygaldry-bindings-portable-session_data
            )
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ${lib}
        PRIVATE sygaldry-helpers-endpoints
        )
target_include_directories(${lib}-test PRIVATE ${PROJECT_SOURCE_DIR}/dependencies/rapidjson/include)
catch_discover_tests(${lib}-test)
endif()
