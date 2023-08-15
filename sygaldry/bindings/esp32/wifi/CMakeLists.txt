set(lib sygaldry-bindings-esp32-wifi)

add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
    target_link_libraries(${lib}
        INTERFACE sygaldry-utility_components-portable-cstdio_logger
        INTERFACE sygaldry-helpers-endpoints
        INTERFACE sygaldry-helpers-metadata
        INTERFACE idf::nvs_flash
        INTERFACE idf::esp_wifi
        INTERFACE idf::esp_common
        INTERFACE idf::freertos
        )
target_link_libraries(sygaldry-bindings-esp32 INTERFACE ${lib})
