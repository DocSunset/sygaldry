#pragma once

#include <type_traits>
#include <concepts>

namespace sygaldry { namespace concepts {

template<typename ... L> struct args_list {};

template<typename NotAFunction>
struct function_reflection
{
    using exists = std::false_type;
};

template<typename R, typename... Args>
struct function_reflection<R(Args...)> {
    using exists = std::true_type;
    using return_type = R;
    using args = args_list<Args...>;
    using is_free = std::true_type;
    using is_member = std::false_type;
    using is_static = std::false_type;
    using is_const = std::false_type;
    using is_volatile = std::false_type;
    using is_noexcept = std::false_type;
};

template<typename R, typename... Args>
struct function_reflection<R(*)(Args...)> : function_reflection<R(Args...)> {};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...)> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) const> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_const = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) volatile> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_volatile = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) const volatile> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_const = true;
    static constexpr bool is_volatile = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) noexcept> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_noxcept = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) const noexcept> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_const = true;
    static constexpr bool is_noxcept = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) volatile noexcept> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_volatile = true;
    static constexpr bool is_noxcept = true;
};

template<typename R, typename Class, typename... Args>
struct function_reflection<R(Class::*)(Args...) const volatile noexcept> : function_reflection<R(Args...)>
{
    static constexpr bool is_member = true;
    static constexpr bool is_const = true;
    static constexpr bool is_volatile = true;
    static constexpr bool is_noxcept = true;
};

template<typename ... Args> concept function_reflectable = function_reflection<Args...>::exists::value;

} }
