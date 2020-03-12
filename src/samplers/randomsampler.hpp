#pragma once

// TODO rand for threading

#include <cstdlib>
#include "sampler.hpp"

using std::rand;

class RandomSampler : public Sampler
{
public:
	float get1f() {
		return (float)rand() / RAND_MAX;
	}
	vec2f get2f() {
		return vec2f(get1f(), get1f());
	}
	unsigned get1u(unsigned limit) {
		return rand() % limit;
	}
	vec3f sampleUnitSphereSurface() {
		vec3f v;
		do v = vec3f(get1f()*2-1, get1f()*2-1, get1f()*2-1);
		while (norm(v)>1 || norm(v)<1e-3);
		return normalized(v);
	}
};
