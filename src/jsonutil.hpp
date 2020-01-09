#pragma once

#include <iostream>
#include "lib/json.hpp"
#include "la.h"

using Json = nlohmann::json;

vec3 json2vec3(const Json& arr) {
	std::vector<float> a = arr;
	assert(a.size()==3);
	return vec3(a[0], a[1], a[2]);
}

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
