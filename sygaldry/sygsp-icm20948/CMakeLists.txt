set(lib sygsp-icm20948)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE
        sygah-mimu
        sygah-metadata
        sygsp-delay
        sygsp-delay
        sygsp-micros
        sygsp-mimu_units
        )
