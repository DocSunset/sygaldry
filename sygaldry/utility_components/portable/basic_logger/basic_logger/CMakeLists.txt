set(lib sygaldry-utility_components-portable-basic_logger)

add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
target_link_libraries(sygaldry-utility_components INTERFACE ${lib})
