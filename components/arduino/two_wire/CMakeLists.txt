set(lib sygaldry-components-arduino-i2c)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygaldry-helpers-metadata)

target_link_libraries(sygaldry-components-arduino INTERFACE ${lib})
