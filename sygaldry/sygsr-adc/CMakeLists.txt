set(lib sygsr-adc)
add_library( STATIC)
target_sources( PRIVATE .cpp)
target_include_directories( PUBLIC .)
target_link_libraries(
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(-test .test.cpp)
target_link_libraries(-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(-test PRIVATE )
#target_link_libraries(-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(-test)
endif()
