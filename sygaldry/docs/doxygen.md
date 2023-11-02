\page page-docs-doxygen Literate Programming with Doxygen

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This page is a work in progress.

Use backslashes, not atsigns. The latter would conflict with lili syntax. We
currently prefer `/*!` for opening documentation block comments and `///` for
single line documentation comments. Best stick with this convention for
consistency.

Don't bother documenting endpoints that already have a name, description, and
range, unless there's something unexpected about them.

Components should also be nested within appropriate `doxygen` groups
corresponding to their package namespace and software component identifier.
Components should also have a `doxygen` `\page` command at the top of the file,
where the page name is `page-sygXX-component_name`, i.e. the software component
identifier prepended with `page-`, and the title of the page begins with the
software component identifier, as in `sygXX-component_name: Page Title`. After
the component ID, the page title is often simply the name of the component.
While this is all terribly redundant, it's necessary so that the documentation
website will include the software component identifier in the page title so
that users reading the documentation can unambiguously make this important
association.

Here's a documentation cheatsheet demonstrating all required syntax and
commands for most uses:

```cpp
/*! \addtogroup parent-group-identifier Parent Group Name
*/
/// \{

/*! \defgroup group-identifier Group Name
Documentation of the group goes here. All documented software components should
be placed in a group whose identifier is the same as the software component
id, e.g. `sygaldry-package-platform-component_name`. You could also use \addtogroup id
to add entities to a group that is defined elsewhere.

After the group documentation, start the group as seen on the next lines.
*/
/// \{

// code goes here

/*! \brief A brief description.
Brief descriptions may or may not include multiple lines.
If you forget the `\brief` command, then this paragraph will be considered
part of the detailed description, and no brief will appear in the documentation
where there would otherwise be one, e.g. in summarised listings of entities at
the module level. Remember to add a `\brief` description!

Detailed description begins after the line break that seperates the brief
paragraph from the rest of the documentation. You can use markdown formatting
here for things like *emphasis (usually italic)* **strong emphasis (usually bold)**
***very strong emphasis***, `preformatted (usually monospace)`, [links][url here]
and so on. You can also use doxygen \ref id command to refer to documented entities.
This can also be used [as the destination of a link][\ref id]. \ref commands can be
used almost anywhere. See https://www.doxygen.nl/manual/autolink.html and https://www.doxygen.nl/manual/commands.html#cmdref
for more detail on linking.

\warning Anything important that the user should know e.g. to avoid errors.

\tparam tparamname Template parameter documentation
\param[in] inargname Input argument documentation
\param[out] outargname Documentation of an argument, such as a pointer or reference, that is used as a return path for outputs
\param[in,out] inoutargname Documentation of an argument that is used for both input and output
\pre Documentation of the precondition(s) of a subroutine (e.g. function, method, operator)
\post Documentation of the postcondition(s).
\return Documentation of the return value.
\sa "See also". A list of \ref commands or auto-linkable references to other entities.
*/
template<typename tparamname> int entity_name(auto inargname, auto& outargname, auto& inoutargname);

/// Just a short description of a simple entity. This gets treated as a brief description.
enum class a_simple_enum {
          a_value, ///< Brief documentation following an entity.
    another_value, ///< Prefer to put the documentation before the entity where it makes sense.
      third_value, ///< List-ish values like this are one place where post-comments might be justified.
};

// remember to close all the groups you opened!
/// \}
/// \}
```

- See \ref page-sygah-endpoints for an example of the annotation syntax
- For entities that can be described in one line, use `/// brief description`
- For entities that require more in depth documentation, use `/*!` to begin the
  documentation block and `*/` to close it.
    - Use `\brief` to give a brief description of the entity. The brief doc
      is a whole paragraph that ends with an empty line, see the example.
      implicit syntax forms; this seems less prone to subtle errors leading to
      unexpected documentation formatting, and is (imo) easier to remember.
    - Use `\tparam name description` to document template parameters
    - Use `\param[in,out] name description` to document function and method arguments
    - Use `\return description` to document the return value of functions and methods
    - Use `\pre` and `\post` to document the expected pre- and post-conditions of functions and methods
- Within comment blocks, don't try to maintain a `*` on every line. It's a
  hassle in some text editors when also trying to keep lines under a reasonable
  length, and doesn't really add much.
