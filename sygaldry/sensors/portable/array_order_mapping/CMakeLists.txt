
set(lib sygaldry-sensors-portable-array_order_mapping)
add_library(${lib} STATIC)

add_subdirectory(function)

    target_sources(${lib}
            PRIVATE ${lib}.cpp
            )
    target_include_directories(${lib}
            PUBLIC .
            )
    target_link_libraries(${lib}
            PUBLIC sygaldry-helpers-endpoints
            PUBLIC sygaldry-helpers-metadata
            )
target_link_libraries(sygaldry-sensors-portable INTERFACE ${lib})

# build automation for the test suite goes here

