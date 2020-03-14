#pragma once

#include "../geometry/geometry.hpp"
#include "../color.h"

// BxDF should work on both hemispheres

class BxDF
{
protected:
	bool _isDirac;
	bool _isRefractive;
public:
	bool const& isDirac = _isDirac;
	bool const& isRefractive = _isRefractive;
	// assuming normal N = (0,0,1)
	// for Dirac f should always return 0
	virtual Color f(const vec3f& wo, const vec3f& wi) const = 0;
	// for Dirac f, returns dI/dI, pdf is always assigned 1
	virtual Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler&) const = 0;
};


class BRDF : public BxDF
{
public:
	BRDF()
	{
		_isRefractive = false;
	}
	// default implementation for diffuse reflectives
	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const
	{
		wi = sampler.cosSampleHemisphereSurface();
		pdf = wi.z/PI;
		if (wo.z<0) wi.z *= -1;
		return f(wo, wi);
	}
};


class LambertBRDF : public BRDF
{
private:
	Color albedo;
public:
	LambertBRDF(Color r): albedo(r)
	{
		_isDirac = false;
	}
	Color f(const vec3f& wo, const vec3f& wi) const
	{
		return 1/PI * albedo;
	}
};