#pragma once

#include "bxdf.hpp"


// DO NOT USE in production! Debug use ONLY.
// Objects of bsdf of solely this bxdf are completely invisible.
// The only effect is that efficiency of sampling is reduced.

class InvisibleBTDF : public BTDF, public DiracBxDF
{
public:
	InvisibleBTDF()
	{
		flags = Type(TRANSMIT | DIRAC);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = -wo;
		pdf = 1;
		return 1 / fabs(wo.z);
	}
};