\page page-docs-lili Literate Programming with lili

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

`lili` is a simple literate programming tool that allows machine code to be
embedded in a document in any order, and later extracted for compilation.
This is accomplished by annotating the document with simple command strings
that allow the machine code to be identified and reassembled in order.

`lili` is not a markup language, like LaTeX or Markdown, but it is designed
to be easily embedded in such a language. Sygaldry uses Markdown for its
literate sources.

There are four basic `lili` annotations that most authors will want to use, two
for delimiting the beginning (`@='name'`, `@#'name'`) of chunks of code, one
for marking the end (`@/`), and one for invoking a chunk of code defined
elsewhere (`@{}`). Use of these commands allow the machine code to be
presented in arbitrary order, not limited by the semantics of the compiler.
This enables the author absolute flexibility over the presentation of the
code so that they can describe the code in whatever way is hoped to be most
clear to other human readers.

For more documentation on `lili`, run `lili -h` from
[the nix shell environment](\ref page-docs-developer_setup),
or review [`lili`'s literate source code](https://github.com/DocSunset/lili).

### Chunk definitions

A basic code chunk definition looks like this:

```cpp
// @='chunk name'

/* body of code chunk here */

// @/
```

The sequence of characters `@=` followed by a name in quotes is recognized by
`lili` as the start of a code chunk with the given name. Anything on the line
before the `@` or after the name is ignored. Typically, this line is commented
out in the machine source language (C++ above), but this is purely for the
aesthetic of the typeset documentation. The entire line will never appear in
the extracted machine code.

The sequence `@/` is used to mark the end of the code chunk. As with the start
of the chunk, anything else on the line where `@/` appears is ignored. By
convention, we comment out this line in the source language.

Within the document where the code chunk is defined, the chunk is always
wrapped in appropriate code-fencing syntax so that it will be syntax
highlighted when the documentation website is generated. In Markdown, this is
accomplished with three backticks. The opening code fence can also be marked
with the name of the language, e.g. `cpp` for C++ or `cmake` for CMake, which
enables syntax highlighting in many editors, as well as in the generated
documentation website.

### Chunk invocations

When a chunk is defined anywhere in a document with `@='name'` and `@/`, it can
be invoked in another chunk definition with the sequence `@{name}`. This has
the effect of pasting the chunk in the location of the invocation. Whatever
characters preceed the `@` of the invocation are taken to be indentation, and
every line of the pasted chunk will have this indentation prepended to it.

So if we have a chunk:

```cpp
// @='child chunk'
i += 5;
// @/
```

And we invoke it:

```cpp
// @='parent chunk'
int i = 0;
int j = 0;
while (i < 20)
{
    ++j;
    @{child chunk}
}
printf("%d\n", j);
// @/
```

This should print `4`.

\warning Note that it is only allowed to invoke a chunk once. This is a
deliberate constraint on the design of `lili` to prevent "code reuse by copy
and paste" usage idioms.

### Tangle chunks

The last commonly needed command is used to define a machine code document that
should be extracted from the documentation. It works in the same way as a
regular code chunk definition, but using the sequence `@#` instead of `@=`. The
name of the chunk it taken to be a filename. When `lili` is run, the text of
the chunk thus defined, with any chunk invocations expanded recursively, will
be written to a file with that name, in the same directory as the `lili` source
file. This process is called "tangling" the machine source, which by analogy
exists in an un-tangled state woven through the literate source prose.

Tangle chunks should be enclosed in their own code fence block, as doing so
makes it a bit easier to read these blocks, especially when there are several
one after another as is common towards the bottom of the literate source when
wrapping up with the boring unremarkable details.
