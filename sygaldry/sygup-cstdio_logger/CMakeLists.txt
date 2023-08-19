set(lib sygup-cstdio_logger)

add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygup-basic_logger
        )
