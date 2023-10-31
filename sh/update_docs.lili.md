\page page-sh-update_docs sh/update_docs.sh Documentation Convenience Script

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

In addition to the literate sources, which are intended to document the
implementation and design rationale of the project, public API's are also
documented using Doxygen special comment blocks, and pretty documentation is
generated with Doxygen and Doxygen Awesome CSS.

The order in which files are presented in the generated documentation is
controlled by manually specifying each file as a subpage in
[the implementation guide](\ref docs-implementation),
as well as some pages being ordered depending
on their order in the `doxyfile` `INPUT` option.

Documentation can be generated e.g. by running `doxygen` in the root of the
repository (remember to run `lili.sh` first!), or using `test.sh
_build_doxygen`.

The documentation website is served from a seperate repository. You can update
the documentation website repository with the following script which takes the
sygladry documentation repo's directory as argument, defaulting to
`../sygaldry-docs`.

```sh
# @#'update_docs.sh'
#!/bin/sh -e

# Update the documentation repo at $1

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cd "$SYGALDRY_ROOT"
lili.sh || exit 1
doxygen
docs="_build_doxygen/html"
[ -d "$1" ] && repo="$1" || repo="../sygaldry-docs"
[ -d "$repo" ] || { echo "could not find documentation repository $repo" ; exit 1 ; }
hash="$(git rev-parse HEAD)"
rm -r "$repo/html"
cp -r "$docs" "$repo"
cd "$repo"
git commit --all -m "Update to sygaldry commit $hash (modulo working tree changes)"
git push
# @/
```
