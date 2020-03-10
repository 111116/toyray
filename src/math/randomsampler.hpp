#pragma once

// TODO rand for threading

#include <cstdlib>
#include "vecfloat.hpp"
using std::rand;


float randf()
{
	return (float)rand()/RAND_MAX;
}

vec3f randunitvec3f() {
	vec3f v;
	do v = vec3f(randf()*2-1, randf()*2-1, randf()*2-1);
	while (norm(v)>1 || norm(v)<1e-3);
	return normalized(v);
}
// TODO