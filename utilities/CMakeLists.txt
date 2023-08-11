add_library(sygaldry_utilities INTERFACE)
target_include_directories(sygaldry_utilities INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Utilities ALIAS sygaldry_utilities)
