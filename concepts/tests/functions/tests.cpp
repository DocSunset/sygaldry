#include "concepts/functions.hpp"

using namespace sygaldry::concepts;

// this should not be function reflectable
float not_func;

// the rest should
void free_func(int) {}

// we should know that the rest are members, but void_free_func isn't
struct void_operator { void operator()() {} };

// we should know that this is const, volatile, and noexcept, but void_operator isn't
struct int_main { int main() const volatile noexcept {return 1;} };

// we should know that this one's method is static
struct static_int_main { static int main() {return 1;} };

static_assert(function_reflection<decltype(free_func)>::exists::value);
static_assert(std::same_as<void, function_reflection<decltype(free_func)>::return_type>);
static_assert(std::same_as<args_list<int>, function_reflection<decltype(free_func)>::args>);
static_assert(function_reflection<decltype(free_func)>::is_free::value);
static_assert(not function_reflection<decltype(free_func)>::is_member::value);
static_assert(not function_reflection<decltype(free_func)>::is_static::value);
static_assert(not function_reflection<decltype(free_func)>::is_const::value);
static_assert(not function_reflection<decltype(free_func)>::is_volatile::value);
static_assert(not function_reflection<decltype(free_func)>::is_noexcept::value);

static_assert(std::is_same_v<function_reflection<decltype(&void_operator::operator())>::return_type, void>);
static_assert(std::is_same_v<function_reflection<decltype(&int_main::main)>::return_type, int>);

static_assert(function_reflectable<decltype(&void_operator::operator())>);
static_assert(function_reflectable<decltype(&int_main::main)>);
