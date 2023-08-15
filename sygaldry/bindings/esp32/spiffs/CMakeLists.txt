set(lib sygaldry-bindings-esp32-spiffs)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE ${SYGALDRY_ROOT}/dependencies/rapidjson/include
            INTERFACE .
            )
    target_link_libraries(${lib}
        INTERFACE sygaldry-bindings-portable-rapid_json
        INTERFACE idf::spiffs
        )
target_link_libraries(sygaldry-bindings-esp32 INTERFACE ${lib})
