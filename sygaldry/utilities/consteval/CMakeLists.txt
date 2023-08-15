set(lib sygaldry-utilities-consteval)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygaldry-utilities INTERFACE sygaldry-utilities-consteval)
