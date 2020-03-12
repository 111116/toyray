#pragma once

#include "aabox.hpp"

class Primitive
{
public:
	class Hit;
	// geometries are two-sided
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, Hit* result) const = 0;
	// virtual point surface_uniform_sample(Sampler&) const = 0;
	// virtual float surfaceArea(Sampler&) const = 0;
	virtual AABox boundingVolume() const = 0;
};

class BasicPrimitive;
class Primitive::Hit {
public:
	BasicPrimitive const* primitive = NULL;
	point p;
	operator bool() {
		return primitive!=NULL;
	}
	Hit(){}
	Hit(BasicPrimitive const* primitive, const point& p): primitive(primitive), p(p)	{}
};

// elementary geometric pritimive (instead of composites like mesh)
class BasicPrimitive : public Primitive
{
private:
	virtual bool intersect(const Ray& ray, point* result) const = 0;
public:
	bool intersect(const Ray& ray, Hit* result) const final {
		point p;
		bool t = intersect(ray, &p);
		*result = Hit(this, p);
		return t;
	}
	// normals can be of either sign; should be determined during shading
	virtual vec3f Ns(const point&) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3f Ng(const point&) const = 0; // geometry normal (geometric intrinsic property)
};


