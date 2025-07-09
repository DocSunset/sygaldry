\page page-sygac-functions sygac-functions: Function Reflection

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

We would like to be able to reflect over any function (whether free or member),
access its return and argument types, and tell whether member functions are
const, volatile, and/or noexcept qualified. This document describes the
implementation of function reflection metaprogramming facilities to provide
this useful functionality.

```cpp
// @+'tests'
// this should not be function reflectable
float not_func;

// the rest should
void free_func(int) {}

// we should know that the rest are members, but void free_func isn't
struct void_operator { void operator()() {} };

// we should know that this is const, volatile, and noexcept, but void_operator isn't
struct int_main { int main(int i) const volatile noexcept {return i;} };
// @/
```

# Function Reflection

We need to do some template metaprogramming. We define a function reflection
meta-function (i.e. a template struct used to take in types and return types).
In the base case, when its argument is not a function, it declares that
function reflection doesn't exist for this type by setting its type alias flag
`exists` false.

Note that we use the boolean constant types from `type_traits` instead of
defining e.g. `static constexpr bool exists = false;`. We expect the
`function_reflection` facility to be used in template metaprograms more often than
not, where boolean constant types are more ergonomic.

```cpp
// @='function type reflection base case'
template<typename NotAFunction>
struct function_type_reflection
{
    using exists = std::false_type;
};
// @/
```

We then specialize the struct for the case where the argument *is* a function,
making the return type and list of arguments seperately available as types in
the scope of the template, as well as several flags describing the context and
qualification of the function type.

We define a trivial type list template to
carry around the list of argument types.

```cpp
// @='type list'
/// A trivial type list struct for carrying function argument type lists
template<typename ... Ts> struct function_arg_list {};
// @/
```

Then we define a based case for when the metafunction is given a free function
as an argument. As well as indicating that function type reflection exists,
this also makes the return type and arguments type list readily available, and
indicates that the function is free, not a member function, and therefore that
it is not const, volatile, or noexcept qualified, since these qualifiers only
apply to class member functions.

```cpp
// @='function type reflection function case'
template<typename Ret, typename... Args>
struct function_type_reflection<Ret(Args...)> {
    using exists = std::true_type;
    using return_type = Ret;
    using arguments = function_arg_list<Args...>;
    using is_free = std::true_type;
    using is_member = std::false_type;
    using parent_class = std::false_type; // should we even define this?
    using is_const = std::false_type;
    using is_volatile = std::false_type;
    using is_noexcept = std::false_type;
};
// @/

// @+'tests'
static_assert(function_type_reflection<decltype(free_func)>::exists::value);
static_assert(std::same_as<void, function_type_reflection<decltype(free_func)>::return_type>);
static_assert(std::same_as<function_arg_list<int>, function_type_reflection<decltype(free_func)>::arguments>);
static_assert(function_type_reflection<decltype(free_func)>::is_free::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_member::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_const::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_volatile::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_noexcept::value);
// @/
```

So that our reflection mechanism will also work with pointers to functions and
member functions, we specialize the template for these cases, as well as
adding additional information for cv-noexcept qualified member function. We are
able to save ourselves a bit of repetition by inheriting the basic `exists`,
`return_type`, and `arguments` type aliases from the base function case, shadowing
the default `false` flags where the specialization would have them `true`.

```cpp
// @='function type reflection specializations'
template<typename Ret, typename... Args>
struct function_type_reflection<Ret(*)(Args...)> : function_type_reflection<Ret(Args...)> {};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...)> : function_type_reflection<Ret(Args...)>
{
    using is_free = std::false_type;
    using is_member = std::true_type;
    using parent_class = Class;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) const> : function_type_reflection<Ret(Class::*)(Args...)>
{
    using is_const = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) volatile> : function_type_reflection<Ret(Class::*)(Args...)>
{
    using is_volatile = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) const volatile> : function_type_reflection<Ret(Class::*)(Args...)>
{
    using is_const = std::true_type;
    using is_volatile = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) noexcept> : function_type_reflection<Ret(Class::*)(Args...)>
{
    using is_noexcept = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) const noexcept> : function_type_reflection<Ret(Class::*)(Args...)const>
{
    using is_noexcept = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) volatile noexcept> : function_type_reflection<Ret(Class::*)(Args...)volatile>
{
    using is_noexcept = std::true_type;
};

template<typename Ret, typename Class, typename... Args>
struct function_type_reflection<Ret(Class::*)(Args...) const volatile noexcept> : function_type_reflection<Ret(Class::*)(Args...)const volatile>
{
    using is_noexcept = std::true_type;
};
// @/

// @+'tests'
static_assert(std::same_as<void_operator, function_type_reflection<decltype(&void_operator::operator())>::parent_class>);
static_assert(std::same_as<void, function_type_reflection<decltype(&void_operator::operator())>::return_type>);
static_assert(std::same_as<function_arg_list<>, function_type_reflection<decltype(&void_operator::operator())>::arguments>);
static_assert(function_type_reflection<decltype(&void_operator::operator())>::is_member::value);
static_assert(not function_type_reflection<decltype(&void_operator::operator())>::is_free::value);
static_assert(not function_type_reflection<decltype(&void_operator::operator())>::is_const::value);
static_assert(not function_type_reflection<decltype(&void_operator::operator())>::is_volatile::value);
static_assert(not function_type_reflection<decltype(&void_operator::operator())>::is_noexcept::value);

static_assert(std::same_as<int_main, function_type_reflection<decltype(&int_main::main)>::parent_class>);
static_assert(std::same_as<int, function_type_reflection<decltype(&int_main::main)>::return_type>);
static_assert(std::same_as<function_arg_list<int>, function_type_reflection<decltype(&int_main::main)>::arguments>);
static_assert(function_type_reflection<decltype(&int_main::main)>::is_member::value);
static_assert(function_type_reflection<decltype(&int_main::main)>::is_const::value);
static_assert(function_type_reflection<decltype(&int_main::main)>::is_volatile::value);
static_assert(function_type_reflection<decltype(&int_main::main)>::is_noexcept::value);
static_assert(not function_type_reflection<decltype(&int_main::main)>::is_free::value);
// @/
```

Finally, we define a concept to detect when function reflection is available.

```cpp
// @='function reflectable concept'
template<typename ... Args> concept function_type_reflectable = function_type_reflection<Args...>::exists::value;
// @/

// @+'tests'
static_assert(function_type_reflectable<decltype(free_func)>);
static_assert(function_type_reflectable<decltype(&free_func)>);
static_assert(function_type_reflectable<decltype(&void_operator::operator())>);
static_assert(function_type_reflectable<decltype(&int_main::main)>);
// @/
```

We would also like to be able to reflect on function values, such as pointers
to functions, pointer to members, and eventually pointers to function objects,
rather than only on function types. This leads to the following metafunction,
with use of `std::decay` to avoid having to make specializations for every
combination of possibly const value, lvalue reference, and rvalue reference.
Because this metafunction is defined in terms of the previous
`function_type_reflection` facility, we can also more strictly constrain its
inputs to those for which reflection is possible.

```cpp
// @='function reflection'
template<auto f> concept function_reflectable = function_type_reflectable<std::decay_t<decltype(f)>>;

template<auto f>
    requires function_reflectable<f>
struct function_reflection
: function_type_reflection<std::decay_t<decltype(f)>>
{
    using parent_reflection = function_type_reflection<std::decay_t<decltype(f)>>; // for tests
};
// @/

// @+'tests'
constexpr auto fp = &void_operator::operator();
constexpr auto& fpr = fp;
constexpr const auto & cfpr = fp;
static_assert(std::same_as<function_reflection<&void_operator::operator()>::parent_reflection, function_reflection<fp>::parent_reflection>);
static_assert(std::same_as<function_reflection<&void_operator::operator()>::parent_reflection, function_reflection<fpr>::parent_reflection>);
static_assert(std::same_as<function_reflection<&void_operator::operator()>::parent_reflection, function_reflection<cfpr>::parent_reflection>);

static_assert(function_reflectable<free_func>);
static_assert(function_reflectable<&free_func>);
static_assert(function_reflectable<&void_operator::operator()>);
static_assert(function_reflectable<&int_main::main>);
// @/
```

# Summary

```cpp
// @#'sygac-functions.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <type_traits>
#include <concepts>

namespace sygaldry {
/*! \addtogroup sygac
 */
/// \{

/*! \defgroup sygac-functions sygac-functions: Function Reflection
 */
/// \{

@{type list}

@{function type reflection base case}

@{function type reflection function case}

@{function type reflection specializations}

@{function reflectable concept}

@{function reflection}

/// \}
/// \}
}
// @/

// @#'sygac-functions.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygac-functions.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygac-functions)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah)
catch_discover_tests(${lib}-test)
endif()
# @/
```
