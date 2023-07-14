cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS ${SYGALDRY_ROOT}/components/esp32)
set(EXTRA_COMPONENT_DIRS ${SYGALDRY_ROOT}/bindings/esp32)
project(one-bit-bongo)
