set(lib sygsa-icm20948)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib}
        PRIVATE ${lib}-two_wire_serif.cpp
        )
target_link_libraries(${lib}
        PUBLIC sygsp-arduino_hack
        )
