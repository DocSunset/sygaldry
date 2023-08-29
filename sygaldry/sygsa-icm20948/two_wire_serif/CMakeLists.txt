set(lib sygsa-icm20948-two_wire_serif)
add_library(${lib} STATIC)
    target_sources(${lib}
            PRIVATE ${lib}.cpp
            )
    target_include_directories(${lib}
            PUBLIC .
            )
    target_link_libraries(${lib}
            PUBLIC sygsp-arduino_hack
            )
