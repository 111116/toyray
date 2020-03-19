#pragma once

#include <iosfwd>
#include "lib/json.hpp"
#include "math/vecfloat.hpp"

using Json = nlohmann::json;


std::ostream& operator<< (std::ostream& out, Json::value_t t);

bool is_number(const Json& o);

vec3f json2vec3f(const Json& o);

vec2f json2vec2f(const Json& o);
