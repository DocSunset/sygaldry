set(lib sygaldry-sensors-portable-button)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygaldry-utilities-consteval
        PUBLIC sygaldry-helpers-endpoints
        )
target_link_libraries(sygaldry-sensors-portable INTERFACE ${lib})
