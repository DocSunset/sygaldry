set(lib sygsp-complementary_mimu_fusion)
add_library(${lib} STATIC)
target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        PUBLIC sygah-mimu
        PUBLIC sygac-mimu
        PUBLIC sygsp-mimu_units
        PRIVATE Eigen3::Eigen
        )

# TODO: write tests
