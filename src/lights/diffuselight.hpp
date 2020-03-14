#pragma once

#include "light.hpp"

class DiffuseLight : public Light
{
private:
	Color _radiance;
	Primitive* base;

public:
	DiffuseLight(bool sample, const Color& color, Primitive* base):
		Light(sample), _radiance(color), base(base) {}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, Sampler&) const
	{
		// TODO
		throw "DiffuseLight: [sample by source] unimplemented";
	}

	// caller MUST make sure the ray is intersecting the emitter mesh
	Color radiance(const Ray& ray) const
	{
		return _radiance;
	}
};
