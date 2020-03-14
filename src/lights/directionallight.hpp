#pragma once

#include "light.hpp"

class DirectionalLight : public DiracLight
{
private:
	Color irradiance;
	vec3f direction;

public:
	DirectionalLight(const Color& irradiance, const point& direction):
		irradiance(irradiance), direction(normalized(direction)) {}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, float& dist, Sampler&) const
	{
		dirToLight = -direction;
		dist = std::numeric_limits<float>::infinity();
		return irradiance;
	}
};
