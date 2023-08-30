#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-string_literal.hpp"
#include "sygah-consteval.hpp"

namespace sygaldry {

/// \ingroup sygah Sygaldry Helpers
/// \{

/// \defgroup sygah-metadata Metadata Helpers
/// \{

/// Helper macro used in the definition of metadata structs; This get's undefined immediately so don't try to use it!
#define metadata_struct(NAME) template<string_literal str>\
struct NAME##_\
{\
    /*! Returns the NAME metadata string */\
    static _consteval auto NAME() {return str.value;}\
}

// TODO: refactor these so the base classes are capitalised instead of post-underscored?

/// Document the name of an entity, e.g. an endpoint, component, or binding
metadata_struct(name);
/// Document a textual description of an entity, e.g. an endpoint, component or binding
metadata_struct(description);
/// Document a textual description of the unit of measurement used by an entity, especially an endpoint
metadata_struct(unit);
/// Document the author of an entity, e.g. a component or binding
metadata_struct(author);
/// Document the designer of an entity, if different from the author, e.g. designer of an instrument vs author of the firmware
metadata_struct(designer);
/// Document the author's contact email of an entity, e.g. a component or binding
metadata_struct(email);
/// Document the copyright license of an entity, e.g. a component or binding
metadata_struct(license);
/// Document the copyright statement of an entity, e.g. a component or binding
metadata_struct(copyright);
/// Document an unambiguous unique identifier for the type of an entity e.g. a component or binding
metadata_struct(uuid);
/// Document a textual description of the version number of an entity, e.g. a component or binding
metadata_struct(version);
/// Document a textual description of the date when an entity was released, e.g. a component or binding
metadata_struct(date);

#undef metadata_struct

/// \}
/// \}
}
