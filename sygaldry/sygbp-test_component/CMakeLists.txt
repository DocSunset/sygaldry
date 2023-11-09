set(lib sygbp-test_component)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-metadata
        INTERFACE sygah-endpoints
        )
