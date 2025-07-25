/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygac-functions.hpp"

using namespace sygaldry;

// this should not be function reflectable
float not_func;

// the rest should
void free_func(int) {}

// we should know that the rest are members, but void free_func isn't
struct void_operator { void operator()() {} };

// we should know that this is const, volatile, and noexcept, but void_operator isn't
struct int_main { int main(int i) const volatile noexcept {return i;} };
static_assert(function_type_reflection<decltype(free_func)>::exists::value);
static_assert(std::same_as<void, function_type_reflection<decltype(free_func)>::return_type>);
static_assert(std::same_as<function_arg_list<int>, function_type_reflection<decltype(free_func)>::arguments>);
static_assert(function_type_reflection<decltype(free_func)>::is_free::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_member::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_const::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_volatile::value);
static_assert(not function_type_reflection<decltype(free_func)>::is_noexcept::value);
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
static_assert(function_type_reflectable<decltype(free_func)>);
static_assert(function_type_reflectable<decltype(&free_func)>);
static_assert(function_type_reflectable<decltype(&void_operator::operator())>);
static_assert(function_type_reflectable<decltype(&int_main::main)>);
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
