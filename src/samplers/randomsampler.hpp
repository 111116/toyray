#pragma once

// std::rand sampler (for single-threaded)

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
		// TODO optimize
		vec3f v;
		do v = vec3f(get1f()*2-1, get1f()*2-1, get1f()*2-1);
		while (norm(v)>1 || norm(v)<1e-3);
		return normalized(v);
	}
	vec2f sampleUnitTriangle() {
		vec2f t = get2f();
		if (t.x + t.y > 1) {
			t.x = 1 - t.x;
			t.y = 1 - t.y;
		}
		return t;
	}
	vec2f sampleUnitDisk() {
		float x,y;
		do {
			x = get1f()*2-1;
			y = get1f()*2-1;
		}
		while (x*x+y*y>1);
		return vec2f(x,y);
	}
};
