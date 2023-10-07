set(lib sygsa-max17055)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib} PUBLIC sygsp-delay)
# arguably this should be a different library, even in a different document
target_sources(${lib} PRIVATE sygsa-max17055-two_wire_serif.cpp)
target_link_libraries(${lib} PUBLIC sygsp-arduino_hack)
