#pragma once

#include "geometry.hpp"
#include "transform.hpp"

#include "triangle.hpp" // DEBUG

class Quad: public BasicPrimitive
{
public:

	Quad() {}

	bool intersect(const Ray& ray, float& result) const
	{
		float t = -ray.origin.y / ray.dir.y;
		point p = ray.atParam(t);
		if (t>0 && p.x>-0.5 && p.x<0.5 && p.z>-0.5 && p.z<0.5)
		{
			result = t;
			return true;
		}
		return false;
	}

	vec3f Ns(const point& p) const
	{
		return vec3f(0,1,0);
	}

	vec3f Ng(const point& p) const
	{
		return vec3f(0,1,0);
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		vec2f t = sampler.get2f();
		return SampleInfo(vec3f(t.x-0.5, 0, t.y-0.5), vec3f(0,1,0), 1);
	}

	AABox boundingVolume() const
	{
		return AABox(-0.5, 0.5, 0, 0, -0.5, 0.5);
	}

};