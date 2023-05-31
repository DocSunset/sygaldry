cmake_minimum_required(VERSION 3.26)
project(Sygaldry)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

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
include_directories(./)
set(AVENDISH_INCLUDE_SOURCE_ONLY ON)
include_directories(dependencies/avendish/include/)

add_subdirectory(utilities)
add_subdirectory(bindings)
add_subdirectory(instruments)
