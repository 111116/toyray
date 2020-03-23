#pragma once

#include "bxdf.hpp"


class LambertBRDF : public BRDF
{
public:
	LambertBRDF()
	{
		flags = Type(REFLECT);
	}

	Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const override
	{
		return 1/PI * albedo;
	}
};
