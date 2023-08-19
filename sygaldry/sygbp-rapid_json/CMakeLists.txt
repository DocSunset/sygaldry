set(lib sygbp-rapid_json)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE ${SYGALDRY_ROOT}/dependencies/rapidjson/include
            INTERFACE .
            )
    target_link_libraries(${lib}
            INTERFACE sygac-endpoints
            INTERFACE sygac-components
            INTERFACE sygah-metadata
            INTERFACE sygbp-osc_string_constants
            INTERFACE sygbp-session_data
            )
target_link_libraries(sygbp INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ${lib}
        PRIVATE sygah-endpoints
        )
target_include_directories(${lib}-test PRIVATE ${PROJECT_SOURCE_DIR}/dependencies/rapidjson/include)
catch_discover_tests(${lib}-test)
endif()
