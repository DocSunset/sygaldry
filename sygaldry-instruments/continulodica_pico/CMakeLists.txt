cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(sygin-continulodica_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
pico_sdk_init()

add_subdirectory($ENV{SYGALDRY_ROOT} sygbuild)

add_executable(continulodica
    continulodica.cpp
)

#pico_enable_stdio_usb(continulodica ENABLED)

pico_add_extra_outputs(continulodica)

target_link_libraries(continulodica pico_stdlib sygaldry tinyusb_device tinyusb_board)
