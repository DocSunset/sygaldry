set(lib sygaldry-sensors-portable-debug_printer)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygaldry-helpers-metadata
        )

target_link_libraries(sygaldry-sensors-portable INTERFACE ${lib})
