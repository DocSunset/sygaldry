set(lib sygse-adc)

add_library(${lib} INTERFACE)
target_sources(${lib}
        INTERFACE ${lib}.cpp
        )
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE syghe-pins
        INTERFACE idf::esp_adc
        INTERFACE idf::esp_common
        )
