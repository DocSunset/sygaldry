# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(liblo-tests tests.cpp)
target_link_libraries(liblo-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(liblo-tests PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(liblo-tests PRIVATE ${LIBLO_INCLUDE_DIRS})
catch_discover_tests(liblo-tests)
