#pragma once

#include "math/vecfloat.hpp"

typedef vec3f point;

struct Ray
{
	point origin;
	vec3f dir; // must be normalized
	Ray(point p, vec3f d): origin(p), dir(d) {}
	point atParam(float t) const {
		return origin + dir * t;
	}
};
