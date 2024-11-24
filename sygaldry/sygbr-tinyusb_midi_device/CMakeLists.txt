set(lib sygbr-tinyusb_midi_device)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp tusb_descriptors.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        PUBLIC pico_stdlib tinyusb_device tinyusb_board
        )

#if (SYGALDRY_BUILD_TESTS)
#add_executable(${lib}-test ${lib}.test.cpp)
#target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
#target_link_libraries(${lib}-test PRIVATE ${lib})
##target_link_libraries(${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
#catch_discover_tests(${lib}-test)
#endif()
