set(lib sygse-icm20948-tests)
add_library(${lib} INTERFACE)
    target_include_directories(${lib}
            INTERFACE .
            )
    target_link_libraries(${lib}
            INTERFACE sygsa-icm20948
            )
