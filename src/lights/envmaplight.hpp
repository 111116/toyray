#pragma once

#include "light.hpp"
#include "image.hpp"

class EnvLight : public Light
{
private:
	Image* texture;

public:
	EnvLight(Image* texture):
		Light(false), texture(texture) {}

	Color sampleIrradiance(const point& surface, vec3f& dirToLight, float& dist, Sampler&) const
	{
		throw "EnvLight: [sample by source] unimplemented";
	}

	Color radiance(const Ray& ray) const
	{
		return texture->sample(vec2f(
			0.5+(0.5/PI)*atan2(ray.dir.z, ray.dir.x),
			0.5+(1/PI)*acos(ray.dir.y)
		));
	}
};
