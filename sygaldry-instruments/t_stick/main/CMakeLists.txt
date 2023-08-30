idf_component_register(SRCS "t_stick.cpp"
        )
add_subdirectory(../../../ sygbuild)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors" "-ftemplate-backtrace-limit=0")
target_link_libraries(${COMPONENT_LIB} PRIVATE sygaldry)
