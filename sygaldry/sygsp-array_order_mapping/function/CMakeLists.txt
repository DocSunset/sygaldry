set(lib sygsp-array_order_mapping_function)
add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
target_link_libraries(sygsp-array_order_mapping INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah-endpoints)
catch_discover_tests(${lib}-test)
endif()
