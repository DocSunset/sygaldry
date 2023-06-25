# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(liblo-demo demo.cpp)
target_link_libraries(liblo-demo PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(liblo-demo PRIVATE ${LIBLO_INCLUDE_DIRS})
