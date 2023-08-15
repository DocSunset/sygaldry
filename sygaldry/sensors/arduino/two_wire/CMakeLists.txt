set(lib sygaldry-sensors-arduino-two_wire)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygaldry-helpers-metadata)

target_link_libraries(sygaldry-sensors-arduino INTERFACE ${lib})
