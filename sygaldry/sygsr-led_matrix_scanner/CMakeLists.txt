set(lib sygsr-led_matrix_scanner)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE hardware_gpio
        )
