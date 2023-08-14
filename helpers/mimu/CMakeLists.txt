set(lib sygaldry-helpers-mimu)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygaldry-helpers-metadata)
target_link_libraries(${lib} INTERFACE sygaldry-helpers-endpoints)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygaldry-helpers INTERFACE ${lib})

add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE sygaldry-helpers-mimu
        PRIVATE sygaldry-concepts-mimu
        )
catch_discover_tests(${lib}-test)
