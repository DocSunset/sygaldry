\page bind_osc_match_pattern OSC Address Pattern Matching

An OSC address (of an OSC Methd) is a null terminated string beginning with a
`/` and consisting of `/` seperated ASCII-encoded parts, similar to a URL, that
doesn't include any of the special characters `#*,?[]{}`. An address pattern
matches an address if both are the same, or if each `/` seperated part of the
address matches the corresponding `/` seperated part of the pattern. The parts
match according to the following rules given in the OSC 1.0 spec (Matt Wright).

## OSC Matching Rules

The implementation consists of a switch statement over the pattern with cases
for each special character. Each case should increment the address past all
characters that match the part of the pattern handled by that case (e.g. past
the glob, wildcard, set, or substrings), and increment the pattern past the
part of the pattern handled by that case, then recursively calling the pattern
matcher. Recursion terminates when a mismatch is found, returning false, or
when both the pattern and address points reach their null terminators at the
same time.

```cpp
// @='osc_match_pattern'
/// Match an OSC address pattern against the given address
bool osc_match_pattern(const char * pattern, const char * address)
{
    switch (*pattern)
    {
    @{glob case}
    @{set case}
    @{substrings case}
    case '\0':
        if (*address == '\0') return true;
        break;
    @{descendant-or-self case}
    @{default case}
    }
    return false;
}
// @/
```

## Wildcard

Wildcards: A in the pattern `?` matches any single character in the address. This
is handled by a branch in the default case that matches if the pattern is `?` or
if the pattern and address are the same.

```cpp
// @+'tests'
TEST_CASE("OSC match wildcards")
{
    CHECK(osc_match_pattern("/???", "/123"));
    CHECK(osc_match_pattern("/foo.?", "/foo.8"));
    CHECK(not osc_match_pattern("/foo.?", "/foo.42"));
}
// @/

// @='default case'
default:
    if (*pattern == '?' || *pattern == *address) return osc_match_pattern(++pattern, ++address);
    else return false;
    break;
// @/
```

## Glob

Globs: A `*` in the pattern matches any sequence of zero or more characters in
the address. The spec is not entirely unambiguous about how globs should work.
Here, multiple sequential globs are considered equivalent to one and the
glob matches as many characters as possible until the address reaches a `/`,
its null terminator, or a character that matches the next in the pattern.

```cpp
// @+'tests'
TEST_CASE("OSC match globs")
{
    CHECK(osc_match_pattern("/*", "/123"));
    CHECK(not osc_match_pattern("/*", "/123/456"));
    CHECK(osc_match_pattern("/foo.*", "/foo.42"));
    CHECK(osc_match_pattern("/foo.*", "/foo."));
    CHECK(osc_match_pattern("/*.bar", "/.bar"));
    CHECK(osc_match_pattern("/*.bar", "/foo.bar"));
    CHECK(osc_match_pattern("/*.bar", "/baz.bar"));
    CHECK(osc_match_pattern("/*/123", "/banana/123"));
    CHECK(osc_match_pattern("/**456", "/123456"));
    CHECK(not osc_match_pattern("/**456", "/foo.bar")); // two asterisks should not form a super-glob
    CHECK(not osc_match_pattern("/*/*", "/banana"));
    CHECK(not osc_match_pattern("/banana/*", "/apple/pie"));
}
// @/

// @='glob case'
case '*':
    while (*pattern == '*') ++pattern;
    while (*address && *address != '/' && *address != *pattern) ++address;
    return osc_match_pattern(pattern, address);
// @/
```

## Sets

Sets: A string of characters enclosed in `[]` brackets in the pattern matches
*one* character in the address if it is found inside the brackets. Note that
this differs from standard regular expressions, where such constructions match
one or more characters.

The spec doesn't explicitly say what to do about a single `[` without a
matching `]`, or given e.g. `[[set]` or `[set]]` whether the the unmatched
bracket should be considered as part of the set or outside of it. Suppose that
an unmatched bracket is treated as a normal character. Since OSC addresses are
forbidden from having brackets anyways, any single bracket not part of a pair
guarantees that the pattern will not match. Then the most error-tolerant way to
handle constructions like `[[set]` and `[set]]` is to pair the outermost
brackets, since the extra one is arguably most likely a typo. By including it
as part of the set, the rest of the set can still match as usual without the
erroneous extra bracket causing a guaranteed match failure. However, this
incurs a penalty in terms of the simplicity and performance of the
implementation. Particularly handling an unmatched closing bracket requires
searching for a possible extra closing bracket, which in most cases will
require a scan of the entire pattern even though no extra closing bracket will
be found. This is considered unacceptable.

Our chosen strategy is to assume that all open brackets will match the next
closing bracket. Extra open brackets within a set are effectively ignored. A
lone closing bracket is treated like a normal character, which should always
trigger a match failure for all well formed OSC addresses. A lone open bracket
will also reach a failure condition when the matching closing bracket is not
located.

```cpp
// @+'tests'
TEST_CASE("OSC match sets")
{
    CHECK(osc_match_pattern("/[123]23", "/123"));
    CHECK(osc_match_pattern("/[123]23", "/223"));
    CHECK(osc_match_pattern("/[123]23", "/323"));
    CHECK(not osc_match_pattern("/[123]23", "/423"));

    CHECK(osc_match_pattern("/[[123]23", "/323")); // by sheer dumb luck
    CHECK(not osc_match_pattern("/[123]]23", "/323"));
    CHECK(not osc_match_pattern("/[12323", "/323"));
    CHECK(not osc_match_pattern("/123]23", "/323"));
}
// @/
```

Within a set, two characters seperated by a `-` indicate the ASCII collating
sequence of characters starting with the first character and ending with the
last character, inclusive. A `-` at the beinning or end of a set has no special
interpretation.

The spec is not clear how to handle a range such as `[z-a]` where the beginning
and end are not in ASCII order. Liblo treats this as matching `z` and `a` and
nothing else. The CNMAT OSC library for Arduino requires the first character in
the range to be `<=` the address and address to be `<=` the second, so nothing
would match this range, i.e. `[z-a]` is an empty range. Since such a range is
arguably invalid, this seems a reasonable approach, and indeed the GNU `grep`
command on my system emits an error given such a range, which may be considered
as a `false` condition. However, in the context of OSC pattern matching, it may
be difficult to propagate such an error back to the user so that they can
resolve it.

The question here is really "what is the user's intent?" It seems like there
are really only two possibilities. Either this is a mistake, and the user's
actual intent is unspecified by such a pattern, or the user meant to request
the range from `z` to `a` inclusive. There's no particular reason why a range
should not go backwards; one can point left *or* right and say "from here to
there". For this reason, and because it can be implemented very easily, we
treat such a range as equivalent to `[a-z]`. In day-to-day use this will likely
almost never come up anyways.

```cpp
// @+'tests'
TEST_CASE("OSC match ranges")
{
    CHECK(osc_match_pattern("/[1-9]", "/1"));
    CHECK(osc_match_pattern("/[1-9]", "/5"));
    CHECK(osc_match_pattern("/[1-9]", "/9"));
    CHECK(osc_match_pattern("/[9-1]", "/5"));
    CHECK(not osc_match_pattern("/[1-9]", "/a"));
    CHECK(osc_match_pattern("/[-1]", "/-"));
    CHECK(osc_match_pattern("/[-1]", "/1"));
    CHECK(osc_match_pattern("/[1-]", "/-"));
    CHECK(osc_match_pattern("/[1-]", "/1"));
    CHECK(not osc_match_pattern("/[-1]", "/2"));
    CHECK(not osc_match_pattern("/[1-]", "/2"));
}
// @/
```

Inverted sets: If the first character within brackets is `!`, this inverts the
meaning of the set so that it matches any character *not* given by the
expression inside the brackets.

```cpp
// @+'tests'
TEST_CASE("OSC match inverted sets")
{
    CHECK(not osc_match_pattern("/[!123]23", "/123"));
    CHECK(not osc_match_pattern("/[!123]23", "/223"));
    CHECK(not osc_match_pattern("/[!123]23", "/323"));
    CHECK(osc_match_pattern("/[!123]23", "/423"));
    CHECK(not osc_match_pattern("/[!1-9]", "/1"));
    CHECK(not osc_match_pattern("/[!1-9]", "/5"));
    CHECK(not osc_match_pattern("/[!1-9]", "/9"));
    CHECK(osc_match_pattern("/[!1-9]", "/a"));
    CHECK(not osc_match_pattern("/[!-1]", "/-"));
    CHECK(not osc_match_pattern("/[!-1]", "/1"));
    CHECK(not osc_match_pattern("/[!1-]", "/-"));
    CHECK(not osc_match_pattern("/[!1-]", "/1"));
    CHECK(osc_match_pattern("/[!-1]", "/2"));
    CHECK(osc_match_pattern("/[!1-]", "/2"));
    CHECK(osc_match_pattern("/[whatever!]", "/!"));
}
// @/
```

The first thing to check in the implementation is whether there is an
exclaimation mark at the beginning of the set. We set the variable `negate`
accordingly. This will be used at the end of the subroutine to influence
the interpretation of a match found within the set.

We then search for a character or range that matches the current character in
the address. This is done with an inner loop.

The inner loop leaves the pattern pointing to the closing bracket if no match
is found, or otherwise to the character following the match. We fast forward
the pattern pointer so that it always points to the closing bracket before
recusing. If this fast forwarding brings the pattern to its null-terminator,
the set is malformed and we short-circuit returning false.

```cpp
// @='set case'
case '[':
{
    bool negate = *++pattern == '!';
    bool match = false;
    while (not match)
    {
        @{set loop}
    }
    while (*pattern && *pattern != ']') ++pattern;
    if (*pattern == '\0') return false;
    if (negate) match = not match;
    if (match) return osc_match_pattern(++pattern, ++address);
    else return false;
    break;
}
// @/
```

Within the set matching loop, we short-circuit in case the pattern reaches its
null terminator. This suggests that the pattern included an open bracket
without a paired closing bracket, which is an error resulting in a guaranteed
match failure.

If the closing bracket is found, we break out of the loop with the current value
of match; since this happens at the beginning of the loop, this suggests that
a match was not found.

```cpp
// @+'set loop'
if (*pattern == '\0') return false;
if (*pattern == ']') break;
// @/
```

We then check for a range, handling it appropriately. 
The check for a range should make the reader a bit nervous. How can we be sure
that we have not dereferenced memory that is out of bounds? The structure of
the check protects us. We know that `*pattern` is not null from above, so it is
safe to check `*(pattern + 1)`. If this is the null terminator, the check will
fail and the second operand of the `&&` will not be executed. If it is not the
null terminator, then we know it is safe to check `*(pattern + 2)`.

```cpp
// @+'set loop'
if (*(pattern + 1) == '-' && (*(pattern + 2) && *(pattern + 2) != ']'))
{
    char start = *pattern;
    char end = *(pattern + 2);
    pattern = pattern + 3;
    if (start < end) match = start <= *address && *address <= end;
    else             match = end   <= *address && *address <= start;
}
// @/
```

In case we do not encounter a range, we check if the current character of the
address matches the current regular character in the set, incrementing the
pattern.

```cpp
// @+'set loop'
else if (*pattern++ == *address) match = true;
// @/
```

## Substrings

Substrings: A comma-separated list of strings enclosed in curly braces (e.g.,
“{foo,bar}”) in the pattern matches any one of the strings in the list
found in the address.

The same considerations as for mismatched brackets in sets apply equally to
mismatched braces in substrings, and the same implementation strategy is
adopted.

```cpp
// @+'tests'
TEST_CASE("OSC match substrings")
{
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/apple/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/banana/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/blueberry/pie"));
    CHECK(osc_match_pattern("/{{apple,banana,blueberry}/pie", "/banana/pie"));
    CHECK(not osc_match_pattern("/{apple,banana,blueberry}/pie", "/sugar/pie"));

    CHECK(osc_match_pattern("/{{apple,banana,blueberry}/pie", "/banana/pie")); // sheer dumb luck
    CHECK(not osc_match_pattern("/{apple,banana,blueberry}}/pie", "/banana/pie"));
    CHECK(not osc_match_pattern("/{apple,banana,blueberry/pie", "/banana/pie"));
}
// @/
```

The structure of this case is similar to that of sets, with an inner loop that
searches for a match, and with two initial branches in the loop that break out
if the null terminator of the pattern is reached or the end of the construct is
reached without finding a match. Also shared, after the loop the pattern is
fast forwarded to the end of the construct before recursing or returning false.

```cpp
// @='substrings case'
case '{':
{
    ++pattern;
    const char * address_start = address;
    bool match = false;
    while (not match)
    {
        if (*pattern == '\0') return false;
        if (*pattern == '}') break;
        @{substrings loop}
    }
    while (*pattern && *pattern != '}') ++pattern;
    if (*pattern == '\0') return false;
    if (match) return osc_match_pattern(++pattern, address);
    else return false;
    break;
}
// @/
```

The inner loop has two main cases. If the pattern and address match, then
we continue to scan them. If the scan reaches the end of a substring
(a comma seperating two substrings or the end of the substrings construct),
then we have found a match.

```cpp
// @='substrings loop'
if (*pattern++ == *address++)
{
    if (*pattern == ',' || *pattern == '}') match = true;
}
// @/
```

Otherwise, if the pattern and address don't match, we reset the address to the
character it pointed to on entry to the substrings construct, and fast forward
the pattern to the end of the construct or the beginning of the next substring
before continuing the loop, possibly matching against a later substring in the
comma-seperated list.

```cpp
// @+'substrings loop'
else
{
    address = address_start;
    while (*pattern && *pattern != '}' && *pattern != ',') ++pattern;
    if (*pattern == ',') ++pattern;
}
// @/
```

## Normal characters

Normal characters: Any other character in the pattern matches the same
character in the address. This is handled by the default case presented
above, which also handles wildcards.

```cpp
// @+'tests'
TEST_CASE("OSC match regular")
{
    CHECK(osc_match_pattern("/sugar/pie", "/sugar/pie"));
    CHECK(not osc_match_pattern("/sugar/pie", "/apple/pie"));
}
// @/
```

## Descendant-or-Self Wildcard

[The OSC 1.1 paper by Adrian Freed and Andy Schmeder](https://zenodo.org/record/1177517)
\cite freed2009nime_osc1.1 suggests the addition of a multi-level wildcard `//`
that matches "across disparate branches of the address tree and at any depth."
The operator is borrowed from [XPath](https://www.w3.org/TR/xpath-31/#abbrev),
where it is described in section 3.3.5. The behavior of the operator is most
readily explained by demonstration.

```cpp
// @+'tests'
TEST_CASE("OSC match descendant-or-self wildcard")
{
    CHECK(osc_match_pattern("//foo", "/foo"));
    CHECK(osc_match_pattern("//foo", "/123/foo"));
    CHECK(osc_match_pattern("//foo", "/123/456/foo"));
    CHECK(osc_match_pattern("//foo", "/bar/baz/foo"));
    CHECK(not osc_match_pattern("//foo", "/bar"));
    CHECK(not osc_match_pattern("//foo", "/foo/bar"));
    CHECK(osc_match_pattern("/banana//pie", "/banana/pie"));
    CHECK(osc_match_pattern("/banana//pie", "/banana/cream/pie"));
    CHECK(osc_match_pattern("/banana//pie", "/banana/coconut/pie"));
    CHECK(osc_match_pattern("/banana//pie", "/banana/coconut/cream/pie"));
    CHECK(not osc_match_pattern("/banana//pie", "/apple/pie"));

    // terminating // not allowed
    CHECK(not osc_match_pattern("//", "/anything"));
    CHECK(not osc_match_pattern("/anything//", "/anything"));

    // three /// or more in a row is an error with undefined behavior
    // our implementation treats this as equivalent to //
    CHECK(osc_match_pattern("///foo", "/foo"));
    CHECK(osc_match_pattern("///foo", "/anything/foo"));
}
// @/
```

The implementation first begins by checking that we have a `//` and not
merely a `/`. In the latter case, we deliberately fall through to the
default case.

```cpp
// @='descendant-or-self case'
case '/':
    if (*(pattern+1) != '/') /*fallthrough*/;
// @/
```

Otherwise, we try to match the remainder of the pattern against the
rest of the address. If this is unsuccessful, we fast forward the
address to the leading `/` of the next part of the address and try
to match again. This proceeds until a match is found, prompting the
matching routine to return true, or until the end of the address
is reached, meaning the pattern did not match.

```cpp
// @+'descendant-or-self case'
    else
    {
        ++pattern;
        while (not osc_match_pattern(pattern, address))
        {
            while (*address)
            {
                ++address;
                if (*address == '/') break;
            }
            if (*address == '\0') return false;
        }
        return true;
    }
    [[fallthrough]];
// @/
```

# Summary

```cpp
// @#'tests/osc_match_pattern/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "bindings/osc_match_pattern.hpp"

@{tests}
// @/

// @#'bindings/osc_match_pattern.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// TODO: this should probably not be a header only implementation
bool osc_match_pattern(const char * pattern, const char * address);
// @/

// @#'src/osc_match_pattern.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "bindings/osc_match_pattern.hpp"

@{osc_match_pattern}
// @/
```

```cmake
# @#'tests/osc_match_pattern/CMakeLists.txt'
add_executable(osc-match-tests tests.cpp)
target_link_libraries(osc-match-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(osc-match-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(osc-match-tests)
# @/
```
