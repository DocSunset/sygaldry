add_library(sygaldry_components STATIC src/trill_craft.cpp)
target_link_libraries(sygaldry_components PUBLIC Sygaldry::Helpers)
add_library(Sygaldry::Components ALIAS sygaldry_components)
