set(lib sygse-gpio)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
