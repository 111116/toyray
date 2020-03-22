#pragma once

#include "bxdf.hpp"


class LambertBRDF : public BRDF
{
private:
	Color albedo;
	
public:
	LambertBRDF(Color r): albedo(r)
	{
		flags = Type(REFLECT);
	}

	Color f(const vec3f& wo, const vec3f& wi) const
	{
		return 1/PI * albedo;
	}
};
