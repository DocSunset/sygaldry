set(lib sygse-max17055)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygse-arduino_hack
        PUBLIC sygsa-max17055
        )
