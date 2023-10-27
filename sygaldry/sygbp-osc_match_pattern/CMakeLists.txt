set(lib sygbp-osc_match_pattern)

add_library(${lib} STATIC)
    target_sources(${lib} PRIVATE ${lib}.cpp)
    target_include_directories(${lib} PUBLIC .)
target_link_libraries(sygbp INTERFACE ${lib})

if(SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
