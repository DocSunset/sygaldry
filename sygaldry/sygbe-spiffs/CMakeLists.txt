set(lib sygbe-spiffs)

add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE ${SYGALDRY_ROOT}/dependencies/rapidjson/include
            INTERFACE .
            )
    target_link_libraries(${lib}
        INTERFACE sygbp-rapid_json
        INTERFACE idf::spiffs
        )
target_link_libraries(sygbe INTERFACE ${lib})
