# Concepts

The concepts library defines the expected conventions for components and
bindings. As such, it is the root of the `sygaldry` framework, and used
extensively by components and bindings alike, but especially by bindings and
other components with throughpoints and plugins. This document provides an
overview of the conceptual framework that guides the concepts library, and
`sygaldry` as a whole by extension.

# Components

A digital musical instrument is an assemblage of components that generate,
process, and output various forms of information with numerous different
structures and temporal characteristics. We define a *component* as a part of a
musical instrument assemblage that implements a specific characteristic
functionality or meaningfully associated group of functionalities. Components
themselves may contain subcomponents that are used to implement their
functionality, or their whole functionality may directly derive from their
subcomponents.

We presently model two kinds of components depending on how they are
structured. A regular component defines a main subroutine, and may optionally
also define input/output structures and/or a parts structure. The functionality
of a regular component is determined by its main subroutine. A pure assembly
defines only a parts structure, and no input/output structures or main
subroutine. Its functionality is defined by running the main subroutines of its
parts. All components must define (or inherit) a static method `name()` that
returns something like a string.

# Ports

We define a *port* as a point in the flow of data that is associated with a
certain component, and can be said to be a source or destination of data
flowing through overall assemblage. We define an *endpoint* as a port with
which metadata is readily associated, such as a name and range. We define
a *throughpoint* as a port that inherits its semantic interpretation from
the source or destination of the data flowing through it.

## Endpoints

Imagine we have two components, `A` and `B`, where `A` is connected directly to
`B`. We call the endpoints of `A` from which data originate its *source
endpoints*. These endpoints can generally be given descriptive names related to
their interpretation with respect to `A`, e.g. "button state" if `A` is a
button, or "saw wave" if `A` is an oscillator. From `A`'s perspective, the data
coming from these endpoints are its *outputs*. Similarly, the endpoints of `B`
where data arrives are its *destination endpoints*, they can be described based
on their interpretation with respect to `B`, e.g. "bypass switch" or "cutoff
frequency", and from `B`'s perspective the data arriving at these endpoints are
its *inputs*. Notice that the data that `A` sees as outputs are the same data
that `B` sees as inputs. However, even through the data may be identical, `A`
and `B` have differing perspectives on the semantics and interpretation of that
information. The interpretation of endpoints depends only on the component with
which they are associated.

Endpoints are declared within the simple aggregate `inputs` and `outputs`
structures of a component. Each endpoint within these structures must be a
unique type, which is easy enough to achieve using template classes and/or
inheritance. This is necessary to allow implementation of throughpoint
combinators. Input and output structures should also be permitted to
recursively contain simple aggregate structures that can optionally be named,
allowing to build hierarchical namespaces of endpoints, although TODO this has
not been implemented yet.

## Throughpoints

Now suppose instead that we have a component `B` that implements a cross-modal
mapping from the output endpoints of `A` to the input endpoints of `C` based on
a training dataset that encodes a certain useful mapping. Information flows
from `A`'s outputs to `B`'s inputs, is processed by `B` according to its
internal learned model, and then continues from `B`'s outputs to `C`'s inputs.
As in the prior example, the data flowing from `A` to `B` is identical at `A`'s
outputs and `B`'s inputs, but whereas in the prior example `B` had a unique
semantic interpretation of that data associated with its input ports, in this
example `B`'s input ports adopt `A`'s perspective, and the interpretation of
the data is the same. Similarly, the data at `B`'s output ports are interpreted
according to `C`'s view of the data. `B`'s ports in this example are defined as
*throughpoints*. Whereas shifts in semantic interpretation are implied by
passing endpoints, semantic interpretation flows unchanged past throughpoints.

TODO A component's throughpoints are declared by first using class template
parameters that accept `inputs` and `outputs` struct types, and then accepting
structs of those types by reference as arguments to the component's main
subroutine. `outputs` originating from a source component should be accepted as
constant reference arguments, while `inputs` to a destination component should
be accepted as mutable reference arguments. TODO A variety of combinator
templates will be provided to allow the combination and filtering out of
endpoints from one or more components before passing them to a throughpoint.

# Subassemblies and Subcomponents

A component that is itself assembled with other components is said to be a
*subassembly*, by analogy with a product subassembly in a larger product, or
sometimes more generically as a super-component. The other components that make
up a subassembly are called *subcomponents*. A subcomponent is called a *part*,
in keeping with the product design analogy, if and only if the subassembly is
solely responsible for managing the subcomponent. A subcomponent that the
assmebly uses but is not solely responsible for is called a *plugin*.

## Parts

Parts are declared similarly to endpoints, as plain member variables of a
simple aggregate `parts` structure of the subassembly component, allowing the
subassembly's parts to be reflected over recursively. Subassemblies are
required to initialize and manage all interactions with their parts, but the
ports of their parts TODO are expected to be exposed by bindings from the
subassembly. This exposure will TODO be controlled through a variety of
mechanisms to be determined later.

## Plugins

Often it is necessary for a subassembly to make use of a subcomponent, but it's
not possible for the subassembly to completely manage the subcomponent, e.g.
especially when doing so requires to draw in unecessary platform-specific
dependencies. This situation is handled similarly to throughpoints, described
above. The required component is declared as a class template type parameter,
an instance of which type is passed to the subassembly component's main
subroutine as a mutable reference. This pattern is termed an *external plugin*.
Alternatively, the plugin type parameter can be used to directly instantiate
an instance of the plugin in the subassembly's parts structure. This pattern
is termed an *internal plugin*.
