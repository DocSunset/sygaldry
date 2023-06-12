cmake_minimum_required(VERSION 3.26)
project(Sygaldry)
set(SYGALDRY_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

    function(add_esp32_project)
    if (${ARGC} LESS 1)
        message(FATAL_ERROR "add_esp32_project requires a project directory")
    endif()

    set(esp32_project_name ${ARGV0})

    if (NOT IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${esp32_project_name})
        message(FATAL_ERROR "add_esp32_project: argument ${CMAKE_CURRENT_LIST_DIR}/${esp32_project_name} is not a valid directory")
    endif()

    if (ESP_PLATFORM)
        return() # avoid recursively building ESP-IDF projects as external projects of themselves
    endif()

    message(STATUS ${esp32_project_name} "ESP32 project added")

    include(ExternalProject)

    ExternalProject_Add(${esp32_project_name}
            PREFIX ${esp32_project_name}
            BUILD_ALWAYS ON
            INSTALL_COMMAND ${SYGALDRY_ROOT}/sh/esp32-install.sh ${esp32_project_name} make flash # TODO: determine generator
            SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${esp32_project_name}
            CMAKE_CACHE_ARGS -DSYGALDRY_ROOT:STRING=${SYGALDRY_ROOT})
endfunction()

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

include_directories(dependencies/pfr/include)
include_directories(dependencies/mp11/include)
include_directories(./)
set(AVENDISH_INCLUDE_SOURCE_ONLY ON)
include_directories(dependencies/avendish/include/)

add_subdirectory(utilities)
add_subdirectory(concepts)
add_subdirectory(helpers)
add_subdirectory(components)
add_subdirectory(bindings)
add_subdirectory(instruments)
add_subdirectory(tests)
