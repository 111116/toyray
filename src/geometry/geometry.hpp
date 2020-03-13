#pragma once

#include "aabox.hpp"

class Primitive
{
public:
	class Hit;
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, Hit* result) const = 0;
	// virtual point surface_uniform_sample(Sampler&) const = 0;
	// virtual float surfaceArea(Sampler&) const = 0;
	virtual AABox boundingVolume() const = 0;
};


class Primitive::Hit {
public:
	bool hit = false;
	point p;
	vec3f Ns, Ng;
	operator bool() {
		return primitive!=NULL;
	}
	Hit(){}
};


// elementary geometric pritimive (instead of composites like mesh)
// used for fast intersection without computing surface normal
class BasicPrimitive : public Primitive
{
public:
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, float& result) const = 0;
	virtual bool intersect(const Ray& ray, Hit* result) const = 0;
	// normals can be of either sign; should be determined during shading
	virtual vec3f Ns(const point&) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3f Ng(const point&) const = 0; // geometry normal (geometric intrinsic property)
};


