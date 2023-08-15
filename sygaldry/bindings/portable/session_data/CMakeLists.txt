set(lib sygaldry-bindings-portable-session_data)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
target_link_libraries(sygaldry-bindings-portable INTERFACE ${lib})
