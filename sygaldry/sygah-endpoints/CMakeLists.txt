set(lib sygah-endpoints)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygah-consteval)
target_link_libraries(${lib} INTERFACE sygah-metadata)
target_include_directories(${lib} INTERFACE .)

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE sygah-endpoints
        PRIVATE sygac-metadata
        PRIVATE sygac-endpoints
        )
catch_discover_tests(${lib}-test)
endif()
