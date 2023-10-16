set(lib sygsp-button)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygah-consteval
        PUBLIC sygah-endpoints
        )
target_link_libraries(sygsp INTERFACE ${lib})

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
