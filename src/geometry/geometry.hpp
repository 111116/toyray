#pragma once

#include "aabox.hpp"
#include "samplers/sampler.hpp"


class Primitive
{
public:
	class Hit;
	class SampleInfo;
	// result shouldn't be changed if there's no hit
	virtual bool intersect(const Ray& ray, Hit* result) const = 0;
	// virtual point surface_uniform_sample(Sampler&) const = 0;
	// virtual float surfaceArea(Sampler&) const = 0;
	virtual AABox boundingVolume() const = 0;
	// pdf is d(probability) / d(area)
	virtual SampleInfo sampleSurface(Sampler& sampler) const = 0;
};


class Primitive::Hit
{
public:
	bool hit = false;
	point p; // hit point coordinate
	vec3f Ns, Ng; // shading normal, geometric normal
	vec2f uv; // texture coordinate
	vec3f dpdu, dpdv; // Jacobian matrix
	Hit(){}
	Hit(const point& p, const vec3f& Ns, const vec3f& Ng, const vec2f& uv):
		hit(true), p(p), Ns(Ns), Ng(Ng), uv(uv) {}
	operator bool() {
		return hit;
	}
};


class Primitive::SampleInfo
{
public:
	point p;
	vec3f normal; // geometric normal; can be either-sided
	float pdf;
	SampleInfo(point p, vec3f normal, float pdf):
		p(p), normal(normal), pdf(pdf) {}
};




