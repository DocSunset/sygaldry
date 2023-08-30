set(lib sygse-micros)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib} PUBLIC sygsp-micros sygse-arduino_hack)
