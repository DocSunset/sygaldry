set(lib sygaldry-sensors-portable-arduino_hack)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygaldry-sensors-portable INTERFACE ${lib})
