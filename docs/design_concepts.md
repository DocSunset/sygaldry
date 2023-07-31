# Concepts

The concepts library defines the expected conventions for components and
bindings. As such, it is the root of the `sygaldry` framework, and used
extensively by components and bindings alike, but especially by bindings and
other components with *throughpoints* and *plugins* (described later). This
document provides an overview of the conceptual framework that guides the
concepts library, and `sygaldry` as a whole by extension.

# Components

A digital musical instrument (DMI) is an assemblage of components that
generate, process, and output various forms of information with numerous
different structures and temporal characteristics, with the purpose of creating
musically meaningful signals (for some interpretation of what constitutes
meaning). We define a *component* as a part of a DMI assemblage that implements
a specific characteristic functionality or meaningfully associated group of
functionalities. Components themselves may contain subcomponents that are used
to implement their functionality, or their whole functionality may directly
derive from their subcomponents.

# Ports

We define a *port* as a point in the flow of data that is associated with a
certain component, and can be said to be a source or destination of data
flowing through the overall assemblage. We define an *endpoint* as a port with
which metadata is readily associated, such as a name and range. We define
a *throughpoint* as a port that inherits its semantic interpretation from
the source or destination of the data flowing through it.

C++ concepts for differentiating different kinds of ports (especially endpoints)
are provided in [the endpoints concepts document](concepts/endpoints.lili.md)

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
information. The interpretation of an endpoint depends only on the component
with which it is associated.

We model endpoints as being declared within the simple aggregate `inputs` and
`outputs` structures of a component. Input and output structures should ideally
be permitted to recursively contain simple aggregate structures that can
optionally be named, allowing to build hierarchical namespaces of endpoints,
although TODO this has not been implemented yet due to challenges related to
type reflection in C++ (see below).

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

A component's throughpoints are declared by first using class template
parameters that accept `outputs` and/or `inputs` struct types that represent
the source and destination of the throughpoints. Then the component must accept
structs of those types by reference as arguments to its main subroutine.
`outputs` originating from a source component should be accepted as constant
reference arguments, while `inputs` to a destination component should be
accepted as mutable reference arguments. TODO A variety of combinator templates
will be provided to allow the combination and filtering out of endpoints from
one or more components before passing them to a throughpoint. E.g. the template
arguments for inputs and outputs may be endpoint structures, or tuples of
endpoints.

# Subassemblies and Subcomponents

A component that is itself assembled from other components is said to be a
*subassembly*, emphasizing its role in the overall DMI assemblage by analogy
with a product subassembly in a larger product, or as a super-component,
emphasizing its role as a container of other components. The other components
that make up a subassembly are called *subcomponents*. A subcomponent is called
a *part*, in keeping with the product design analogy, if and only if the
subassembly is solely responsible for managing the subcomponent. A subcomponent
that the assmebly uses but is not solely responsible for is called a *plugin*.

Component concepts are provided in
[the related document](concepts/components.lili.md)

## Parts

Parts are declared similarly to endpoints, as plain member variables of a
simple aggregate `parts` structure of the subassembly component, allowing the
subassembly's parts to be reflected over recursively. Subassemblies are
required to initialize and manage all interactions with their parts, but the
ports of their parts are expected to be exposed by bindings from the
subassembly. This exposure will eventually be controlled through a variety of
mechanisms to be determined later, so that certain endpoints may be hidden
from the default external exposure.

Often it is unnecessary for a subassembly to know all the details about all of
its parts, and by using some of its parts implicitly, platform-dependencies can
be avoided. When the type of one or more of a subassembly's parts is declared
as a template type parameter of the subassembly, and an instance of the type is
declared in the subassembly's parts structure, this pattern is termed a *part
parameter*.

## Plugins

Often it is necessary for a subassembly to make use of a subcomponent, but
without being completely responsible for managing the subcomponent, e.g.
especially when doing so requires to draw in unecessary platform-specific
dependencies, and when the plugin may have other reponsibilities beyond those
known to the plugin user. This situation is handled similarly to throughpoints,
described above. The required component is declared as a class template type
parameter, an instance of which type is passed to the subassembly component's
main subroutine as a mutable reference. This pattern is termed a *plugin*.

# The Component Tree and the Runtime

The design of many useful DMIs can be fully expressed by a declarative list of
their subcomponents and the throughpoint, part parameter, and plugin
dependencies between them, e.g. [the
T-Stick](/instruments/t_stick/t_stick.lili.md). This kind of declarative
assemblage, realized as a simple aggregate structure of components, can be
decomposed into a tree-like structure called *the component tree*, which
enables various forms of filtering and searching. Using type reflection to
recognize throughpoint and plugin types, these dependencies can be
automatically propagated to components that need them by extracting references
to the required dependencies from the component tree. Template metaprogramming
machinery that implements this technique is described in
[the runtime document](concepts/runtime.lili.md).
