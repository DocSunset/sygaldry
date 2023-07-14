# RapidJSON ESP32 Component

The below CMakeLists.txt file declares an ESP-IDF component by simply
identifying the relevant include directory for RapidJSON.

```cmake
# @#'rapidjson/CMakeLists.txt'
idf_component_register(INCLUDE_DIRS "${SYGALDRY_ROOT}/dependencies/rapidjson/include")
# @/
```
