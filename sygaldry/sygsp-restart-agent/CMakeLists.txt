set(lib sygsp-restart-agent)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE 
        sygah-endpoints
        sygah-metadata
        sygsp-micros
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
#target_link_libraries(${lib}-test PRIVATE sygsp-micros)
catch_discover_tests(${lib}-test)
endif()
