set(lib sygsr-button)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-metadata
        INTERFACE sygsp-button
        INTERFACE hardware_gpio
        )
