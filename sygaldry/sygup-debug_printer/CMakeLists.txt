set(lib sygsp-debug_printer)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-metadata
        )

target_link_libraries(sygsp INTERFACE ${lib})
