set(lib sygbp-osc_match_pattern)

add_library(${lib} INTERFACE)
target_sources(${lib} INTERFACE ${lib}.cpp)
target_include_directories(${lib} INTERFACE .)

if(SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
