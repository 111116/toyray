#pragma once

#include <iostream>
#include "lib/json.hpp"
#include "math/vecfloat.hpp"

using Json = nlohmann::json;



std::ostream& operator<< (std::ostream& out, Json::value_t t) {
	switch (t) {
		case Json::value_t::null: out << "null"; break;
		case Json::value_t::object: out << "object"; break;
		case Json::value_t::array: out << "array"; break;
		case Json::value_t::string: out << "string"; break;
		case Json::value_t::boolean: out << "boolean"; break;
		case Json::value_t::number_integer: out << "number_integer"; break;
		case Json::value_t::number_unsigned: out << "number_unsigned"; break;
		case Json::value_t::number_float: out << "number_float"; break;
		case Json::value_t::discarded: out << "discarded"; break;
	}
	return out;
}

bool is_number(const Json& o) {
	return (o.type() == Json::value_t::number_integer) ||
		   (o.type() == Json::value_t::number_unsigned) ||
		   (o.type() == Json::value_t::number_float);
}

vec3f json2vec3f(const Json& o) {
	if (is_number(o))
		return vec3f(o);
	if (o.type() != Json::value_t::array)
		throw "failed convert json to vec3f";
	std::vector<float> a = o;
	assert(a.size()==3);
	return vec3f(a[0], a[1], a[2]);
}
