idf_component_register(SRCS "one-bit-bongo.cpp"
        )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors")
