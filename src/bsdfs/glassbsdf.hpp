#pragma once

#include "color.h"
#include "bsdf.hpp"

class BxDF;

// wrapper of low-level BxDFs
class GlassBSDF : public BSDF
{
private:
	float IOR;
	// energy here instead of radiance
	float reflect(const vec2f& wo, vec2f& wi);
	float refract(const vec2f& wo, vec2f& wi);

public:
	GlassBSDF(float IOR): IOR(IOR) {}

	virtual Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
	{
		return 0;
	}

	virtual Color sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler&) const;
};
