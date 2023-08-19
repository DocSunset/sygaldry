set(lib sygbp-cstdio_reader)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    #target_sources(${lib}
            #PRIVATE ${lib}.cpp
            #)
