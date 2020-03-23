#pragma once

#include "bxdf.hpp"


class MirrorBRDF : public BRDF, public DiracBxDF
{
	
public:
	MirrorBRDF()
	{
		flags = Type(DIRAC | REFLECT);
	}

	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = vec3f(-wo.x, -wo.y, wo.z);
		pdf = 1;
		return 1 / fabs(wi.z) * albedo;
	}
};