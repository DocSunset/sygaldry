set(lib sygse-pin-driver-array)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE sygse-gpio
        INTERFACE sygup-cstdio_logger
        )
