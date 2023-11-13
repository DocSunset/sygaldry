set(lib sygse-delay)
add_library(${lib} INTERFACE)
target_sources(${lib} INTERFACE ${lib}.cpp)
target_link_libraries(${lib} INTERFACE sygsp-delay sygse-arduino_hack)
