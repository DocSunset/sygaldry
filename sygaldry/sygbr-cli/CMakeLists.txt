set(lib sygbr-cli)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE
        pico_stdio
        sygbr-cli
        sygup-cstdio_logger
        )
