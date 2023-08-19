set(lib sygah-metadata)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygah-consteval)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygah INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE sygah-metadata
        )
catch_discover_tests(${lib}-test)
endif()
