
set(lib sygsp-array_order_mapping)
add_library(${lib} STATIC)

add_subdirectory(function)

target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

# build automation for the test suite goes here

