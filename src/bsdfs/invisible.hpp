#pragma once

#include "bxdf.hpp"


// specular transmission

class InvisibleBTDF : public BTDF, public DiracBxDF
{
public:
	InvisibleBTDF()
	{
		flags = Type(TRANSMIT | DIRAC);
	}

	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = -wo;
		pdf = 1;
		return 1 / fabs(wo.z) * albedo;
	}
};