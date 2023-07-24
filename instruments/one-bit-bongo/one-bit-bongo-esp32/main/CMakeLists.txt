idf_component_register(SRCS "one-bit-bongo.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        INCLUDE_DIRS "." ${SYGALDRY_ROOT}
        )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors")
