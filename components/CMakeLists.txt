add_library(sygaldry_components STATIC src/button.cpp)
target_link_libraries(sygaldry_components PUBLIC sygaldry-helpers)
target_include_directories(sygaldry_components PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Components ALIAS sygaldry_components)

add_subdirectory(trill_craft)
add_subdirectory(arduino-hack)

if (ESP_PLATFORM)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/esp32")
endif()
