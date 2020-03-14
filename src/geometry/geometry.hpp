#pragma once

#include "aabox.hpp"
#include "../samplers/sampler.hpp"


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
		return hit;
	}
	Hit(){}
	Hit(const point& p, const vec3f& Ns, const vec3f& Ng):
		hit(true), p(p), Ns(Ns), Ng(Ng) {}
};


// elementary geometric pritimive (instead of composites like mesh)
// used for fast intersection without computing surface normal
class BasicPrimitive : virtual public Primitive
{
public:
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, Hit* result) const final;
	virtual bool intersect(const Ray& ray, float& result) const = 0;
	// normals can be of either sign; should be determined during shading
	virtual vec3f Ns(const point&) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3f Ng(const point&) const = 0; // geometry normal (geometric intrinsic property)
};


bool BasicPrimitive::intersect(const Ray& ray, Hit* result) const
{
	float t;
	bool b = intersect(ray, t);
	if (!b) return false;
	point p = ray.atParam(t);
	*result = Hit(p, Ns(p), Ng(p));
	return true;
}


class SurfaceSamplablePrimitive : virtual public Primitive
{
public:
	class SampleInfo;
	// pdf is d(probability) / d(area)
	virtual SampleInfo sampleSurface(Sampler& sampler) const = 0;
};

class SurfaceSamplablePrimitive::SampleInfo
{
public:
	point p;
	vec3f normal; // geometric normal; can be either-sided
	float pdf;
	SampleInfo(point p, vec3f normal, float pdf):
		p(p), normal(normal), pdf(pdf) {}
};


