#pragma once

#include "math/matfloat.hpp"
#include "jsonutil.hpp"


mat4f parseTransform(const Json& conf) {
	mat4f trans = mat4f::unit;
	if (conf.find("scale") != conf.end()) {
		vec3f scale = json2vec3f(conf["scale"]);
		trans[0][0] = scale.x;
		trans[1][1] = scale.y;
		trans[2][2] = scale.z;
	}
	if (conf.find("rotation") != conf.end()) {
		std::cerr << "ROTATION UNIMPLEMENTED\n";
		throw -1;
	}
	if (conf.find("position") != conf.end()) {
		vec3f offset = json2vec3f(conf["position"]);
		trans[0][3] = offset.x;
		trans[1][3] = offset.y;
		trans[2][3] = offset.z;
	}
	return trans;
}