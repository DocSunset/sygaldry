#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <tuple>
#include <type_traits>
#include <concepts>

namespace sygaldry {
/*! \addtogroup sygac
 */
/// \{

/*! \defgroup sygac-functions sygac-functions: Function Reflection
 */
/// \{


template<typename NotAFunction>
struct function_type_reflection
{
    using exists = std::false_type;
};

template<typename Ret, typename... Args>
struct function_type_reflection<Ret(Args...)> {
    using exists = std::true_type;
    using return_type = Ret;
    using arguments = std::tuple<Args...>;
    using is_free = std::true_type;
    using is_member = std::false_type;
    using parent_class = std::false_type;
    using is_const = std::false_type;
    using is_volatile = std::false_type;
    using is_noexcept = std::false_type;
};

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

template<typename ... Args> concept function_type_reflectable = function_type_reflection<Args...>::exists::value;

template<auto f> concept function_reflectable = function_type_reflectable<std::decay_t<decltype(f)>>;

template<auto f>
    requires function_reflectable<f>
struct function_reflection
: function_type_reflection<std::decay_t<decltype(f)>>
{
    using parent_reflection = function_type_reflection<std::decay_t<decltype(f)>>; // for tests
};

/// \}
/// \}
}
