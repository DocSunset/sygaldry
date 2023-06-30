# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(bindings-demo demo.cpp)
target_link_libraries(bindings-demo PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(bindings-demo PRIVATE ${LIBLO_INCLUDE_DIRS})
