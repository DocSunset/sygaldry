set(lib sygaldry-bindings-portable-cstdio_reader)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    #target_sources(${lib}
            #PRIVATE ${lib}.cpp
            #)
target_link_libraries(sygaldry-bindings INTERFACE ${lib})
