#pragma once

#include "light.hpp"

class DiffuseLight : public Light
{
private:
	Color _radiance;
	SurfaceSamplablePrimitive* base;

public:
	DiffuseLight(bool sample, const Color& color, Primitive* base):
		Light(sample), _radiance(color), base(dynamic_cast<SurfaceSamplablePrimitive*>(base))
	{
		if (base == NULL && samplable)
			throw "DiffuseLight: primitive not samplable";
	}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, float& dist, Sampler& sampler) const
	{
		assert(samplable);
		auto info = base->sampleSurface(sampler);
		dist = norm(info.p - surface);
		dirToLight = normalized(info.p - surface);
		return fabs(dot(info.normal, dirToLight)) / sqrlen(info.p - surface) / info.pdf * _radiance;
	}

	// caller MUST make sure the ray is intersecting the emitter mesh
	Color radiance(const Ray& ray) const
	{
		return _radiance;
	}
};
