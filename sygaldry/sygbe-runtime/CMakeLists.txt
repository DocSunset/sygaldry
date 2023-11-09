set(lib sygbe-runtime)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac-runtime
        INTERFACE sygsa-two_wire
        INTERFACE sygbe-spiffs
        INTERFACE sygbe-wifi
        INTERFACE sygbp-liblo
        INTERFACE sygbp-cli
        )
