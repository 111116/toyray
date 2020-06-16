#pragma once

#include "geometry.hpp"


// elementary geometric pritimive (instead of composites like mesh)
// used for fast intersection without computing surface normal
class BasicPrimitive : public Primitive
{
public:
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, Hit* result) const final;
	virtual bool intersect(const Ray& ray, float& result) const = 0;
	// normals can be of either sign; should be determined during shading
	virtual vec3f Ns(const point&) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3f Ng(const point&) const = 0; // geometry normal (geometric intrinsic property)
	virtual vec2f uv(const point&) const = 0; // geometry normal (geometric intrinsic property)
};


bool BasicPrimitive::intersect(const Ray& ray, Hit* result) const
{
	float t;
	bool b = intersect(ray, t);
	if (!b) return false;
	point p = ray.atParam(t);
	*result = Hit(p, Ns(p), Ng(p), uv(p));
	return true;
}




