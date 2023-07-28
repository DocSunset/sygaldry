idf_component_register( SRCS ${SYGALDRY_ROOT}/../SparkFun_ICM-20948/src/ICM_20948.cpp
                             ${SYGALDRY_ROOT}/../SparkFun_ICM-20948/src/util/ICM_20948_C.c
                        INCLUDE_DIRS ${SYGALDRY_ROOT}/../SparkFun_ICM-20948/src
                                     ${SYGALDRY_ROOT}/../SparkFun_ICM-20948/src/util
                        REQUIRES arduino-hack
                      )
