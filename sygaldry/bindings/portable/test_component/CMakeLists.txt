set(lib sygaldry-bindings-portable-test_component)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
    target_link_libraries(${lib}
            INTERFACE sygaldry-helpers-metadata
            INTERFACE sygaldry-helpers-endpoints
            )
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})
