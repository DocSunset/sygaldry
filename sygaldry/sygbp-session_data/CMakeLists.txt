set(lib sygbp-session_data)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
target_link_libraries(sygbp INTERFACE ${lib})
