set(lib sygsp-button)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_sources(${lib} INTERFACE ${lib}.cpp)
target_link_libraries(${lib}
        INTERFACE sygah-consteval
        INTERFACE sygah-endpoints
        )

if(SYGALDRY_BUILD_AVENDISH)
avnd_make_pd(TARGET ${lib}
        MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${lib}.hpp"
        MAIN_CLASS sygaldry::sygsp::ButtonGestureModel
        C_NAME sygsp_button
        )
target_link_libraries(${lib}_pd
        PUBLIC ${lib}
        PUBLIC sygah-consteval
        PUBLIC sygah-endpoints
        )
endif()
