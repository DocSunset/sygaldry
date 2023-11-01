\page page-sh-new_component sh/new_component.sh New Component Convenience Script

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This script provides a simple interactive dialog that facilitates setting up
a new component.

```sh
# @#'new_component.sh'
#!/bin/sh -e
function usage() {
    cat <<USAGE
$0 [options] - new component setup script
    -p PACKAGE      name of the package for the new component, e.g. sygsp
    -c COMPONENT_ID name of the software component in the component identifier,
                    e.g. "button" (as in "sygsp-button")
    -t TITLE        title of literate source documentation page,
                    e.g. "Portable Button"
    -n NAME         Human-readable name of the component, e.g. "Button"
    -s STRUCT_NAME  C++ type name of the component struct, e.g. "Button"
                    (as in `struct Button ...`)
    -d DESCRIPTION  brief description of the software component
    -a AUTHOR       Full name of the author
    -l LICENSE      SPDX license tag, e.g. MIT

The script will attempt to create a new component, adding it to the build
system, and filling in the provided information. If any information is
missing, the script will prompt the user to fill in the blanks.
USAGE
}

while [ "$#" -gt 0 ]
do
case $1 in
    "-p") shift ; PACKAGE="$1" ; shift ;;
    "-c") shift ; COMPONENT_ID="$1" ; shift ;;
    "-t") shift ; TITLE="$1" ; shift ;;
    "-n") shift ; NAME="$1" ; shift ;;
    "-s") shift ; STRUCT_NAME="$1" ; shift ;;
    "-d") shift ; DESCRIPTION="$1" ; shift ;;
    "-a") shift ; AUTHOR="$1" ; shift ;;
    "-l") shift ; LICENSE="$1" ; shift ;;
    *)
        echo "unrecognized argument $1 in $@@"
        usage
        exit
        ;;
esac
done

[ -z "$PACKAGE" ]      && { echo "What is the package group ID? e.g. sygsp" ;                  read PACKAGE ; }
[ -z "$COMPONENT_ID" ] && { echo "What is the component ID?" ;                                 read COMPONENT_ID ; }
[ -z "$TITLE" ]        && { echo "What should be the title of the literate source document?" ; read TITLE ; }
[ -z "$NAME" ]         && { echo "What should be the human-readable name of the component?" ;  read NAME ; }
[ -z "$STRUCT_NAME" ]  && { echo "What should be the C++ struct type name of the component?" ; read STRUCT_NAME ; }
[ -z "$DESCRIPTION" ]  && { echo "Please provide a brief description of the component: " ;     read DESCRIPTION ; }
[ -z "$AUTHOR" ]       && { echo "What is the full name of the author of the component?" ;     read AUTHOR ; }
[ -z "$LICENSE" ]      && { echo "What is the SPDX license tag of the component?" ;            read LICENSE ; }

echo "executing equivalent to $0 -p '$PACKAGE' -c '$COMPONENT_ID' -t '$TITLE' -n '$NAME' -s '$STRUCT_NAME' -d '$DESCRIPTION' -a '$AUTHOR' -l '$LICENSE'"

COMPONENT="$PACKAGE-$COMPONENT_ID"
YEAR="$(date +%Y)"
FULL_AUTHOR="$(grep -F "$AUTHOR" "$SYGALDRY_ROOT/sygaldry/docs/authors.md")"
[ -z "$FULL_AUTHOR" ] && FULL_AUTHOR="$AUTHOR"
COPYRIGHT="$(echo "Copyright $YEAR $FULL_AUTHOR" | fold -w 80 -s)"
LICENSE="SPDX-License-Identifier: $LICENSE"

echo "summary:"
echo "COMPONENT is '$COMPONENT'"
echo "TITLE is '$TITLE'"
echo "NAME is '$NAME'"
echo "STRUCT_NAME is '$STRUCT_NAME'"
echo "DESCRIPTION is '$DESCRIPTION'"
echo "AUTHOR is '$AUTHOR'"
echo "FULL_AUTHOR is '$FULL_AUTHOR'"
echo "LICENSE is '$LICENSE'"

implementation_docs="$SYGALDRY_ROOT/sygaldry/docs/implementation.md"
component_dir="$SYGALDRY_ROOT/sygaldry/$COMPONENT"
[ -d "$component_dir" ] &&
    { echo "Directory '$component_dir' already exists! Aborting." ; exit 1 ; }
[ -f "$implementation_docs" ] ||
    { echo "Implementation documentation file not located at '$implementation_docs'! Aborting." ; exit 1 ; }
grep -sq "\($PACKAGE\)" "$implementation_docs" ||
    { echo "Package $PACKAGE not found in implementation documentation! Aborting." ; exit 1 ; }
grep -sq "$COMPONENT" "$implementation_docs" &&
    { echo "Component already added to implementation docs! Aborting." ; exit 1 ; }

echo "making directory '$component_dir'..."
mkdir "$component_dir"

echo "adding component to implementation documentation..."
sed -i -e '/('"$PACKAGE"')/a- \\subpage page-'"$COMPONENT" "$implementation_docs"

echo "populating literate source..."
#"$component_dir/$COMPONENT.lili.md"
cat << COMPONENT_LILI_MD > /dev/null
\page page-$COMPONENT $COMPONENT: $TITLE

$COPYRIGHT

SPDX-License-Identifier: $LICENSE

[TOC]

$DESCRIPTION

\`\`\`cpp
// @@#'$COMPONENT.hpp'
#pragma once
/*
$COPYRIGHT

$LICENSE
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace $PACKAGE {

/// \addtogroup $PACKAGE
/// \{

/// \defgroup $COMPONENT $COMPONENT: $TITLE
/// Literate source code: page-$COMPONENT
/// \{

/*! \brief $DESCRIPTION
*/
struct $STRUCT_NAME
: name_<"$NAME">
, description_<"$DESCRIPTION">
, author_<"$AUTHOR">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"$LICENSE">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
    } outputs;

    void init();

    void main();
};

/// \}
/// \}

} }
// @@/
\`\`\`

\`\`\`cpp
// @@#'$COMPONENT.cpp'
/*
$COPYRIGHT

$LICENSE
*/
#include "$COMPONENT.hpp"

namespace sygaldry { namespace $PACKAGE {

void $STRUCT_NAME::init()
{
}

void $STRUCT_NAME::main()
{
}

} }
// @@/
\`\`\`

\`\`\`cpp
// @@#'$COMPONENT.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "$COMPONENT.hpp"

using namespace sygaldry;
using namespace sygaldry::$PACKAGE;

@{tests}
// @@/
\`\`\`

\`\`\`cmake
# @@#'CMakeLists.txt'
set(lib $COMPONENT)
add_library(\${lib} STATIC)
target_sources(\${lib} PRIVATE \${lib}.cpp)
target_include_directories(\${lib} PUBLIC .)
target_link_libraries(\${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(\${lib}-test \${lib}.test.cpp)
target_link_libraries(\${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(\${lib}-test PRIVATE \${lib})
#target_link_libraries(\${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(\${lib}-test)
endif()
# @@/
\`\`\`
COMPONENT_LILI_MD

echo "tangling machine code..."
lili.sh
# @/
```
