#pragma once

#include "aabox.hpp"

class Primitive
{
public:
	// geometries are two-sided
	virtual bool intersect(const Ray& ray, point* result) const = 0;
	// normals can be of either sign; should be determined during shading
	virtual vec3f Ns(const point& p) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3f Ng(const point& p) const = 0; // geometry normal (geometric intrinsic property)
	// virtual point surface_uniform_sample(Sampler&) const = 0;
	// virtual float surfaceArea(Sampler&) const = 0;
	virtual AABox boundingVolume() const = 0;
};

