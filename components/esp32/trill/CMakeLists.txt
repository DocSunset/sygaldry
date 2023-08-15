set(lib sygaldry-components-esp32-trill)
add_library(${lib} STATIC)
target_include_directories(${lib}
        PUBLIC .
        PRIVATE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino
        )
target_sources(${lib}
        PRIVATE ${lib}.cpp
        PRIVATE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp
        )
target_link_libraries(${lib}
        PUBLIC sygaldry-components-esp32-arduino_hack
        PUBLIC sygaldry-components-arduino-trill_craft
        )

target_link_libraries(sygaldry-components-esp32 INTERFACE ${lib})
