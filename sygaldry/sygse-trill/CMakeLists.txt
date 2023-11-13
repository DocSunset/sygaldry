set(lib sygse-trill)
add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE .
        INTERFACE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino
        )
target_sources(${lib}
        INTERFACE ${lib}.cpp
        INTERFACE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp
        )
target_link_libraries(${lib}
        INTERFACE sygse-arduino_hack
        INTERFACE sygsa-trill_craft
        )
