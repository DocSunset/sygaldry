set(lib sygsp-complementary_mimu_fusion)
add_library(${lib} INTERFACE)
target_sources(${lib}
        INTERFACE ${lib}.cpp
        )
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE sygah-mimu
        INTERFACE sygac-mimu
        INTERFACE sygsp-mimu_units
        INTERFACE Eigen3::Eigen
        )

# TODO: write tests
