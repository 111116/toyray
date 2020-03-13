#pragma once

#include "math/matfloat.hpp"
#include "jsonutil.hpp"

namespace {
// Copyright (c) 2014 Benedikt Bitterli <benedikt.bitterli@gmail.com>
// tunabrain/tungsten/src/core/math/Mat4f.cpp
	mat4f rotYXZ(const vec3f &rot)
	{
	    vec3f r = rot*PI/180.0f;
	    float c[] = {std::cos(r.x), std::cos(r.y), std::cos(r.z)};
	    float s[] = {std::sin(r.x), std::sin(r.y), std::sin(r.z)};

	    return mat4f(
	        c[1]*c[2] - s[1]*s[0]*s[2],   -c[1]*s[2] - s[1]*s[0]*c[2], -s[1]*c[0], 0.0f,
	                         c[0]*s[2],                     c[0]*c[2],      -s[0], 0.0f,
	        s[1]*c[2] + c[1]*s[0]*s[2],   -s[1]*s[2] + c[1]*s[0]*c[2],  c[1]*c[0], 0.0f,
	                              0.0f,                          0.0f,       0.0f, 1.0f
	    );
	}
}

mat4f parseTransform(const Json& conf) {
	mat4f trans = mat4f::unit;
	if (conf.find("scale") != conf.end()) {
		vec3f scale = json2vec3f(conf["scale"]);
		trans[0][0] = scale.x;
		trans[1][1] = scale.y;
		trans[2][2] = scale.z;
	}
	if (conf.find("rotation") != conf.end()) {
		// in degrees
		trans = rotYXZ(json2vec3f(conf["rotation"])) * trans;
	}
	if (conf.find("position") != conf.end()) {
		vec3f offset = json2vec3f(conf["position"]);
		trans[0][3] = offset.x;
		trans[1][3] = offset.y;
		trans[2][3] = offset.z;
	}
	return trans;
}