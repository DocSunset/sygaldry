cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(sygin-t_stick_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
pico_sdk_init()

add_subdirectory($ENV{SYGALDRY_ROOT} sygbuild)

add_executable(tstick
    tstick.cpp
)

pico_enable_stdio_usb(tstick 1)

pico_add_extra_outputs(tstick)

target_link_libraries(tstick pico_stdlib sygaldry)
