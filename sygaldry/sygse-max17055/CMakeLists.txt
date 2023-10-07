set(lib sygse-max17055)
add_library(${lib} STATIC)
target_link_libraries(${lib}
        PUBLIC sygse-arduino_hack
        PUBLIC sygsa-max17055
        )

target_link_libraries(sygse INTERFACE ${lib})
