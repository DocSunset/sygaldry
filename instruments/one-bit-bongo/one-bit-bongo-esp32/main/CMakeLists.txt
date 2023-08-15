idf_component_register(SRCS "one-bit-bongo.cpp"
        )
add_subdirectory(${SYGALDRY_ROOT} sygbuild)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors")
target_link_libraries(${COMPONENT_LIB} PRIVATE sygaldry)
