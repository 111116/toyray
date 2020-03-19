#pragma once

#include "light.hpp"

class ConstEnvLight : public Light
{
private:
	Color _radiance;

public:
	ConstEnvLight(const Color& color):
		Light(false), _radiance(color) {}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, float& dist, Sampler&) const
	{
		throw "ConstEnvLight: [sample by source] unimplemented";
	}

	Color radiance(const Ray& ray) const
	{
		return _radiance;
	}
};
