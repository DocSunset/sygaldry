set(lib sygsp-arduino_hack)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygsp INTERFACE ${lib})
