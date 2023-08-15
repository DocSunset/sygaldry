set(lib sygaldry-sensors-esp32-adc)

add_library(${lib} STATIC)
target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygaldry-helpers-endpoints
        PUBLIC sygaldry-helpers-metadata
        )

target_link_libraries(sygaldry-sensors-esp32 INTERFACE ${lib})
