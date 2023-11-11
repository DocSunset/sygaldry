set(lib sygac-tuple)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE . "${SYGALDRY_ROOT}/dependencies/tuplet/include")
target_link_libraries(${lib} INTERFACE Boost::pfr)
