# Literature

-[ ] usage style documentation
-[ ] various empty readme files
-[ ] incorporate [the four types of documentation](https://documentation.divio.com/)
    -[ ] add reference documentation for public interfaces (Doxygen?)
    -[ ] write tutorials
    -[ ] write how-to recipes
    -[ ] contemplate: does the literate source provide sufficient explanation?
-[ ] use inter-file linking

# Build system

-[ ] automate documentation link checking
-[ ] compile lili as a dependency
-[ ] lili should return fail on any warning or error...
-[ ] drive lili tangling from cmake
-[ ] determine ESP32 project build command for esp32-install rather than assuming `make`
-[ ] include boost deps in ESP32 projects automatically, e.g. by linking against top syg proj
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
-[ ] ESP32 ADC
-[ ] Trill on ESP32
-[ ] ICM20948 on ESP32
-[ ] MIMU
-[ ] T-Stick touch gesture
-[ ] Button
    -[ ] tap/hold, tap dance, and other useful mappings
    -[ ] tests
-[ ] ESP32 GPIO hold, sleep hold, and other features not yet implemented or tested

# Bindings

-[ ] CLI on ESP32
-[ ] CLI
    -[ ] write tests for Trigger command
    -[ ] annotate Trigger command
    -[ ] ditch testreader testwriter, just redirect stdio
        -[ ] refactor testreader/writer tests accordingly
    -[ ] provide a full repl experience
