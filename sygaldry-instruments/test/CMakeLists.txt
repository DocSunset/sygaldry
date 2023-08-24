set(exe sygin-test)
add_executable(${exe} sygin-test.cpp)
target_link_libraries(${exe} PRIVATE sygaldry)
