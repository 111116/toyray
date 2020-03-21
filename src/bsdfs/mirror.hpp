#pragma once

#include "bxdf.hpp"


class MirrorBRDF : public BRDF, public DiracBxDF
{
private:
	Color albedo;
	
public:
	MirrorBRDF(Color r): albedo(r)
	{
		assert(isDirac);
		assert(!isRefractive);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = vec3f(-wo.x, -wo.y, wo.z);
		pdf = 1;
		return 1 / fabs(wi.z) * albedo;
	}
};