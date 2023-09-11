set(lib sygsp-icm20948)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib} PUBLIC sygsp-delay sygsp-mimu_units)
# arguably this should be a different library, even in a different document
target_sources(${lib} PRIVATE sygsa-icm20948-two_wire_serif.cpp)
target_link_libraries(${lib} PUBLIC sygsp-arduino_hack)
