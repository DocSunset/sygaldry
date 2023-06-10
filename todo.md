# Literature

-[ ] usage style documentation
-[ ] various empty readme files
-[ ] incorporate [the four types of documentation](https://documentation.divio.com/)
    -[ ] add reference documentation for public interfaces (Doxygen?)
    -[ ] write tutorials
    -[ ] write how-to recipes
    -[ ] contemplate: does the literate source provide sufficient explanation?
-[ ] user inter-file linking

# Build system

-[ ] automate documentation link checking
-[ ] compile litlit as a dependency
-[ ] drive litlit tangling from cmake
-[ ] determine ESP32 project build command for esp32-install rather than assuming `make`
-[ ] automate flashing and running ESP32 tests
-[ ] enforce expected internal library dependency graph using cmake

# Concepts and Helpers

-[ ] subassemblies
-[ ] pure assemblies
-[ ] external plugins
-[ ] enum tag checker
-[ ] message inputs and callback outputs

# Components

-[ ] Liblo on ESP32
-[ ] ESP32 GPIO hold, sleep hold, and other features not yet implemented or tested
-[ ] ESP32 ADC
-[ ] Trill on ESP32
-[ ] T-Stick touch gesture
-[ ] ICM20948 on ESP32
-[ ] MIMU
-[ ] Button
    -[ ] tap/hold, tap dance, and other useful mappings
    -[ ] tests

# Bindings

-[ ] CLI on ESP32
-[ ] CLI
    -[ ] write tests for Trigger command
    -[ ] annotate Trigger command
    -[ ] ditch testreader testwriter, just redirect stdio
        -[ ] refactor testreader/writer tests accordingly
