set(lib sygse-adc)

add_library(${lib} STATIC)
target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        PRIVATE idf::esp_adc
        PRIVATE idf::esp_common
        )
