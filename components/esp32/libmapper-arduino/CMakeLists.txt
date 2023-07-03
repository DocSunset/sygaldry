idf_component_register(SRC_DIRS "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/zlib"
                       INCLUDE_DIRS "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src"
                       PRIV_REQUIRES esp_netif)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DHAVE_CONFIG_H" "-Wno-error=char-subscripts" "-Wno-error=format-truncation" "-Wno-error=format")
