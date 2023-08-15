set(lib sygaldry-bindings-portable-test_reader)

add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
target_link_libraries(sygaldry-bindings INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
catch_discover_tests(${lib}-test)
endif()
