set(lib sygsa-micros)
add_library(${lib} INTERFACE)
target_sources(${lib} INTERFACE ${lib}.cpp)
target_link_libraries(${lib} INTERFACE sygsp-micros sygsp-arduino_hack)
