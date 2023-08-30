set(lib sygsa-delay)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib} PUBLIC sygsp-delay sygse-arduino_hack)
