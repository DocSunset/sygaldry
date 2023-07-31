\page readme_components Components

Components are identifiable functional modules that group together a
characteristic functionality or group of functionalities that the component
provides when used. Examples include sensors, sound synthesis modules, and
mappings. Components are intended to be fairly modular and independent, so that
more complex functionalities can be built up by combining several components
together in an assembly.

Rules for component implementations:

- components should strictly follow [the Sygaldry component structure](concepts/README.md).
    - input endpoints are declared in a simple aggregate member struct called `inputs`
    - ditto outputs
    - subcomponents are declared in a simple aggregate member struct called `parts`
    - components may declare an initialization subroutine `void init(...)`, and a main
      subroutine `void main(...)`
    - that's it! Bindings may reasonably ignore other members of the component, as should assemblies that use the component
        - in other words, everything else is conceptually private. It could even be declared as such, though it doesn't really matter.
- freely use the explicit API of other components in the components library
    - i.e. all components are part of one components library, and our assumptions
      about other components in the library should be enforced within it
    - except don't cross platform boundaries
        - platform independent components may not use platform-dependent components, but platform-specific components may use platform-independent ones.
        - components for one platform may not use components specific to another one.
- components in the component library are not allowed to use components from the binding library, not even as part parameters.
