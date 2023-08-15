set(lib sygaldry-utility_components-portable-test_logger)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    target_link_libraries(${lib}
            INTERFACE sygaldry-utility_components-portable-basic_logger
            )
target_link_libraries(sygaldry-utility_components INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
