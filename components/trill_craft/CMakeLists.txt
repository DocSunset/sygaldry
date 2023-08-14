set(lib sygaldry-components-trill_craft)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygaldry-helpers)
