#pragma once

#include "light.hpp"

class PointLight : public DiracLight
{
private:
	Color intensity;
	point position;

public:
	PointLight(const Color& intensity, const point& position):
		intensity(intensity), position(position) {}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, Sampler&) const
	{
		dirToLight = normalized(position - surface);
		return intensity / sqrlen(position - surface);
	}
};
