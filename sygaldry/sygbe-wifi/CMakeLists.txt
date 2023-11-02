set(lib sygbe-wifi)

add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
    INTERFACE sygup-cstdio_logger
    INTERFACE sygah-endpoints
    INTERFACE sygah-metadata
    INTERFACE idf::nvs_flash
    INTERFACE idf::esp_wifi
    INTERFACE idf::esp_common
    INTERFACE idf::freertos
    )
