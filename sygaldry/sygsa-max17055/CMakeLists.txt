set(lib sygsa-max17055)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygah-metadata sygsp-delay sygsp-micros sygah-endpoints)
# arguably this should be a different library, even in a different document
target_link_libraries(${lib} INTERFACE sygsp-arduino_hack)
