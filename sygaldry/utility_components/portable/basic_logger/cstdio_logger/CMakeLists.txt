set(lib sygaldry-utility_components-portable-cstdio_logger)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    target_link_libraries(${lib}
            INTERFACE sygaldry-utility_components-portable-basic_logger
            )
target_link_libraries(sygaldry-utility_components INTERFACE ${lib})
