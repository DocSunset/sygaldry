#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string_view>
#include <string>
#include <array>
#include "sygah-consteval.hpp"
#include "sygah-metadata.hpp"

namespace sygaldry {

/*! \addtogroup sygah sygah: Sygaldry Helpers
*/
/// \{

/*! \defgroup sygah-endpoints sygah-endpoints: Endpoints Helpers

The endpoint helpers are intended to serve as value-like types. Users can
declare instances of these templates in the `inputs` and `outputs` structures
of a component to add endpoints with convenient APIs, documenting them through
the template parameters of the helper. See any component in the component
library for numerous examples.
*/
/// \{

/*! \defgroup sygah-endpoints-bases sygah-endpoints: Endpoints Bases

Base classes for building endpoints. Advanced users may inherit from these
classes to compose new endpoint types. This is how the canonical endpoint
helpers are implemented.
*/
/// \{

/// A wrapper around a numeric literal that enables using floats as template parameters.
template<typename T> requires std::integral<T> || std::floating_point<T>
struct num_literal
{
    /// The underlying numeric type
    using type = T;
    /// The value of the constant
    T value;
    /// Constructor enabling initialization from a literal constant
    _consteval num_literal(T f) : value{f} {}
    /// Conversion operator so that the `num_literal` can be treated roughly as if it is of `T` type
    operator T() {return value;}
};
/// Document the expected minimum, maximum, and initial value of an endpoint
template<num_literal _min, num_literal _max, num_literal _init = _min>
struct range_
{
    /// Returns a struct with `min`, `max`, and `init` members containing the range
    static _consteval auto range()
    {
        struct {
            decltype(_min.value) min = _min.value;
            decltype(_max.value) max = _max.value;
            decltype(_init.value) init = _init.value;
        } r;
        return r;
    }
    /// Returns the minimum value of the range
    static _consteval auto min() { return _min.value; }
    /// Returns the maximum value of the range
    static _consteval auto max() { return _max.value; }
    /// Returns the initial value of the range
    static _consteval auto init() { return _init.value; }
};
/*! \brief A wrapper around a value-like type

\details `persistent` tries to mimick value semantics, providing constructors
and conversion operators so that a `persistent<T>` can be roughly treated as if
it is a `T`.

Endpoints inheriting from this type should state `using
persistent<T>::operator=` to explicitly inherit the assignment operators from
`persistent`.

*/
template <typename T>
struct persistent
{
    /// The underlying type
    using type = T;
    /// The wrapped value
    T value;
    /// Default constuctor defers to `T`'s empty initializer
    constexpr persistent() noexcept : value{} {}

    // convert from T
    /// Move constructor from `T`
    constexpr persistent(T&& t) noexcept {value = std::move(t);}
    /// Copy constructor from `T`
    constexpr persistent(const T& t) noexcept : value{t} {}
    /// Move assignment from `T`
    constexpr auto& operator=(T&& t) noexcept {value = std::move(t); return *this;}
    /// Copy assignment from `T`
    constexpr auto& operator=(const T& t) noexcept {value = t; return *this;}

    // convert to T
    /// Conversion to a mutable reference to the underlying value
    constexpr operator T&() noexcept {return value;}
    /// Conversion to a constant reference to the underlying value
    constexpr operator const T&() const noexcept {return value;}
};
/*! \brief Wrapper around a type `T` with optional-like semantics and persistent state

\details Intended to provide a means of developing message-like semantics while also
allowing output messages to maintain state between loops, `occasional` provides
a conversion to `bool` that indicates if the underlying wrapped state has been
updated since the last time it was cleared. [The runtime](@ref Runtime) is
meant to clear this flag between loops. However, the underlying state is not
reset in this process.

Endpoints inheriting from this type should state `using
occasional<T>::operator=` to explicitly inherit the assignment operators from
`occasional`.

*/
template <typename T>
struct occasional
{
    /// The underlying type
    using type = T;
    /// The wrapped state
    T state;
    /// Flag indicating if the state has been changed
    bool updated;

    /// Default constructor; `state` is default initialized and `updated` is false
    constexpr occasional() noexcept : state{}, updated{false} {}

    /*! \brief Move constructor from another `occasional`.

    \details Only changes this wrapper if the other one has been updated. `updated` flag
    reflects whether the other `occasional` has been updated.
    */
    constexpr occasional(occasional<T>&& other)
    {
        if (other.updated)
        {
            state = std::move(other.state);
            updated = true;
        } else updated = false;
    }

    /*! \brief Copy constructor from another `occasional`.

    \details Only changes this wrapper if the other one has been updated. `updated` flag
    reflects whether the other `occasional` has been updated.
    */
    constexpr occasional(const occasional<T>& other)
    {
        if (other.updated)
        {
            state = other.state;
            updated = true;
        } else updated = false;
    }

    /*! \brief Move assignment from another `occasional`.

    \details Only changes this wrapper if the other one has been updated. `updated` flag
    reflects whether the other `occasional` has been updated.
    */
    constexpr auto& operator=(occasional<T>&& other)
    {
        if (other.updated)
        {
            state = std::move(other.state);
            updated = true;
        } else updated = false;
        return *this;
    }

    /*! \brief Copy assignment from another `occasional`.

    \details Only changes this wrapper if the other one has been updated. `updated` flag
    reflects whether the other `occasional` has been updated.
    */
    constexpr auto& operator=(const occasional<T>& other)
    {
        if (other.updated)
        {
            state = other.state;
            updated = true;
        } else updated = false; // keep current value
        return *this;
    }

    // convenience access by conversion to underlying type; value semantics
    /// Mutable value access
    constexpr operator T&() noexcept {return state;}
    /// Immutable value access
    constexpr operator const T&() const noexcept {return state;}

    // optional-like semantics
    /// Move constructor from the underlying type
    constexpr occasional(T&& t) noexcept : state{std::move(t)}, updated{true} {}
    /// Copy constructor from the underlying type
    constexpr occasional(const T& t) noexcept : state{t}, updated{true} {}
    /// Move assignment from the underlying type
    constexpr auto& operator=(T&& t) noexcept {state = std::move(t); updated = true; return *this;}
    /// Copy assignment from the underlying type
    constexpr auto& operator=(const T& t) noexcept {state = t; updated = true; return *this;}
    /// Mutable dereference operator; provides access to the underlying state
    constexpr T& operator *() noexcept {return state;}
    /// Constant dereference operator; provides access to the underlying state
    constexpr const T& operator *() const noexcept {return state;}
    /// Mutable member access operator; provides access to the members of the underlying state
    constexpr T* operator ->() noexcept {return &state;}
    /// Constant member access operator; provides access to the members of the underlying state
    constexpr const T* operator ->() const noexcept {return &state;}
    /// Mutable value access
    constexpr T& value() noexcept {return state;}
    /// Immutable value access
    constexpr const T& value() const noexcept {return state;}
    /// Clear the `updated` flag. This can also be achieved by assignment from empty braces, e.g. `x = {};`
    constexpr void reset() noexcept {updated = false;} // maintains current state
};


/// \}

/*! \brief Helper struct for defining recognized tags. This is immediately `undef`ed, so don't try to use it!

\details Tags are indicated by the existence of a bare enumeration value with a certain
name in the scope of a struct. This macro is used to define helper classes that
define a certain enumeration tag. Endpoint types can then inherit the helper
class to mark themselves with the corresponding tag.
*/
#define tag(TAG) struct tag_##TAG {enum {TAG}; }

/*! \brief Write only tag helper

\details Indicates that the value of an endpoint should not be shown to users, e.g. a
WiFi password.
*/
tag(write_only);

/*! \brief Session data tag helper

\details Indicates that the value should be stored persistently across sessions (e.g.
boot cycles) using an appropriate platform-specific method (e.g. EEPROM)
*/
tag(session_data);
#undef tag
/// Apply the tag helpers classes `Tags` to an entity, especially an endpoint
template<typename ... Tags>
struct tagged_ : Tags... {};
/*! \brief A two-state integer endpoint with occasional message semantics
\details Example: `button<"button state", "current state of the button", 1> button_state;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam init The initial value of the endpoint (0 or 1). Defaults to 0.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct button
: occasional<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using occasional<char>::operator=;
};

/*! \brief A two-state integer endpoint with persistent value semantics
\details Example: `toggle<"debounced button state", "current debounced state of the button", 1> debounced_button_state;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam init The initial value of the endpoint (0 or 1). Defaults to 0.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template<string_literal name_str, string_literal desc = "", char init = 0, typename ... Tags>
struct toggle
: persistent<char>
, name_<name_str>
, description_<desc>
, range_<0, 1, init>
, tagged_<Tags...>
{
    using persistent<char>::operator=;
};

/*! \brief A text string endpoint with persistent value semantics
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct text
: persistent<std::string>
, name_<name_str>
, description_<desc>
, tagged_<Tags...>
{
    using persistent<std::string>::operator=;
};

/*! \brief A text string endpoint with occasional message semantics
\details Example: `text_message<"WiFi SSID", "Name of the WiFi network to connect to", tag_session_data> wifi_ssid;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct text_message
: occasional<std::string>
, name_<name_str>
, description_<desc>
, tagged_<Tags...>
{
    using occasional<std::string>::operator=;
};

/*! \brief A numeric endpoint with user customizeable range and persistent value semantics
\details Example: `slider<"pressure", "Current reading from the pressure sensor"> pressure;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam T Underlying value type of the endpoint. Defaults to `float`.
\tparam min Expected minimum value of the endpoint. Defaults to `0.0f`.
\tparam max Expected maximum value of the endpoint. Defaults to `1.0f`.
\tparam init The initial value of the endpoint. Defaults to `min`.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template< string_literal name_str
        , string_literal desc = ""
        , typename T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct slider
: persistent<T>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using persistent<T>::operator=;
};

/*! \brief A numeric endpoint with user customizeable range and occasional value semantics
\details Example: `slider_message<"pressure", "Current reading from the pressure sensor"> pressure;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam T Underlying value type of the endpoint. Defaults to `float`.
\tparam min Expected minimum value of the endpoint. Defaults to `0.0f`.
\tparam max Expected maximum value of the endpoint. Defaults to `1.0f`.
\tparam init The initial value of the endpoint. Defaults to `min`.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template< string_literal name_str
        , string_literal desc = ""
        , typename T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct slider_message
: occasional<T>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using occasional<T>::operator=;
};

/*! \brief A multi-dimensional numeric endpoint with user customizeable range and persistent value semantics
\details Example: `array<"acceleration", 3, "Acceleration due to gravity and motion", float, -1.0f, 1.0f> accl;`
\tparam name_str The name of the endpoint. Required.
\tparam N The number of elements in the array. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam T Underlying value type of the endpoint. Defaults to `float`.
\tparam min Expected minimum value of the endpoint. Defaults to `0.0f`.
\tparam max Expected maximum value of the endpoint. Defaults to `1.0f`.
\tparam init The initial value of the endpoint. Defaults to `min`.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template< string_literal name_str
        , std::size_t N
        , string_literal desc = ""
        , typename T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct array
: persistent<std::array<T, N>>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using persistent<std::array<T, N>>::operator=;
    using type = T;
    constexpr const auto& operator[](std::size_t i) const noexcept
    {
        return persistent<std::array<T, N>>::value[i];
    }
    constexpr auto& operator[](std::size_t i) noexcept
    {
        return persistent<std::array<T, N>>::value[i];
    }
    static _consteval auto size() noexcept
    {
        return N;
    }
};

/*! \brief A multi-dimensional numeric endpoint with user customizeable range and occasional message semantics
\details Example: `array_message<"touch mask", 30, "Mask of currently touched capacitive sensors", unsigned int, 0, 1> touch_mask;`
\tparam name_str The name of the endpoint. Required.
\tparam N The number of elements in the array. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam T Underlying value type of the endpoint. Defaults to `float`.
\tparam min Expected minimum value of the endpoint. Defaults to `0.0f`.
\tparam max Expected maximum value of the endpoint. Defaults to `1.0f`.
\tparam init The initial value of the endpoint. Defaults to `min`.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template< string_literal name_str
        , std::size_t N
        , string_literal desc = ""
        , typename T = float
        , num_literal<T> min = 0.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = min
        , typename ... Tags
        >
struct array_message
: occasional<std::array<T, N>>
, name_<name_str>
, description_<desc>
, range_<min, max, init>
, tagged_<Tags...>
{
    using occasional<std::array<T, N>>::operator=;
    using type = T;
    constexpr const auto& operator[](std::size_t i) const noexcept
    {
        return occasional<std::array<T,N>>::state[i];
    }
    constexpr auto& operator[](std::size_t i) noexcept
    {
        return occasional<std::array<T,N>>::state[i];
    }
    static _consteval auto size() noexcept
    {
        return N;
    }
    void set_updated() noexcept
    {
        occasional<std::array<T,N>>::updated = true;
    }
};
/*! \brief A semantically value-less endpoint that signals an event
\details Example: `bng<"falling edge", "indicates button state falling edge event"> falling_edge;`
\tparam name_str The name of the endpoint. Required.
\tparam desc A description of the endpoint. Defaults to an empty string.
\tparam Tags Tag helper classes to apply to the endpoint. None by default.
*/
template<string_literal name_str, string_literal desc = "", typename ... Tags>
struct bng
: persistent<bool>
, name_<name_str>
, description_<"">
, tagged_<Tags...>
{
    using persistent<bool>::operator=;
    enum {bang, impulse};
    void operator()() {value = true;}
    void reset() {value = false;}
};

/// \}
} // namespaces
