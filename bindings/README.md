\page readme_bindings Bindings

Bindings are components that bind their plugin subcomponents to a certain
environment or protocol, such as Open Sound Control, Pure Data, JUCE, and
others.

Rules for binding implementations:

- bindings are components
- binding subcomponents, such as the logger, are permitted to have idiosyncratic APIs
    - bindings are less well behaved in terms of their semantics than most components
    - components like loggers and so on have unusual characteristics
    - try to stick to the overarching patterns where possible
- freely use the explicit API of other binding components
    - i.e. all bindings are part of one bindings library, and our assumptions
      about other components in the library should be enforced within it
    - except don't cross platform boundaries, as with [components](readme_components).
- generically access component APIs through concepts and reflection
    - i.e. components are considered as part of a seperate library about
      which we should try to avoid making assumptions
    - the assumptions that we do make are meant to be encapsulated and
      isolated in the concepts library, so that changes in those assumptions
      are insulated from the bindings library
