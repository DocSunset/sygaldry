# All

-[ ] clean up using declarations at namespace scope, and namespaces generally

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
-[ ] automatically generate litlit.md files
-[ ] rename litlit to lili?
-[ ] determine ESP32 project build command for esp32-install rather than assuming `make`
-[ ] include deps in ESP32 projects automatically, e.g. by linking against top syg proj
-[ ] automate flashing and running ESP32 tests
-[ ] enforce expected internal library dependency graph using cmake
-[ ] generate a book with a particular linear order to things
    -[ ] latex suitable for inclusion in a phd thesis...
    -[ ] static website

# Concepts and Helpers

-[ ] subassemblies
-[ ] pure assemblies
-[ ] external plugins
-[ ] throughpoints
-[ ] parts parameters
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
    -[ ] provide a full repl experience
