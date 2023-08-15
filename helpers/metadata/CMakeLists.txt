set(lib sygaldry-helpers-metadata)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygaldry-utilities-consteval)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygaldry-helpers INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE sygaldry-helpers-metadata
        )
catch_discover_tests(${lib}-test)
endif()
