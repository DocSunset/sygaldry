set(lib sygsp-continuous-key-scanner)
add_library(${lib} INTERFACE)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        )
