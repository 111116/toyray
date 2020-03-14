#pragma once

#include "../color.h"
#include "../geometry/ray.hpp"
#include "../samplers/sampler.hpp"


// TODO: multiple importance sampling (which may require some refac)

class Light {
public:
	// whether samples should be chosen by light source
	// (currently source samples are EITHER solely chosen by source OR by object)
	const bool samplable;

	// return irradiance or sampled radiance scaled by 1/pdf (sample chosen by source)
	// dirToLight is assigned unit vector
	virtual Color sampleIrradiance(const point& surface, vec3f& dirToLight, Sampler&) const = 0;

	// return radiance (sample chosen by object); zero if dirac
	// ray is inbound (pointed toward light)
	virtual Color radiance(const Ray& ray) const = 0;

protected:
	Light(bool samplable): samplable(samplable) {}
};


class DiracLight : public Light
{
public:
	Color radiance(const Ray& ray) const final
	{
		return 0;
	}
protected:
	DiracLight(): Light(true) {}
};


// class DiffuseAreaLight: public Light {
// 	Color color;
// 	DiffuseAreaLight(Color color): color(color) {
// 	}
// 	Color radiance(const Ray& shadowray) {
// 		return color;
// 	}
// };



