set(lib sygsp-button)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygah-consteval
        PUBLIC sygah-endpoints
        )
target_link_libraries(sygsp INTERFACE ${lib})
