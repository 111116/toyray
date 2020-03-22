#pragma once

#include "geometry/geometry.hpp"
#include "color.h"

// BxDF should work on both hemispheres



class BxDF
{
public:
	enum Type {
		DIRAC = 1,
		REFLECT = 2,
		TRANSMIT = 4
	};
	Type flags;
	bool isDirac() const { return flags & DIRAC; }
	bool isReflective() const { return flags & REFLECT; }
	bool isTransmissive() const { return flags & TRANSMIT; }
	// assuming normal N = (0,0,1)
	// for Dirac f should always return 0
	virtual Color f(const vec3f& wo, const vec3f& wi) const = 0;
	// for Dirac f, returns dI/dI, pdf is always assigned 1
	virtual Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler&) const = 0;
};


class BRDF : virtual public BxDF
{
public:
	BRDF()
	{
		flags = REFLECT;
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


class BTDF : virtual public BxDF
{
public:
	BTDF()
	{
		flags = TRANSMIT;
	}
	// default implementation for diffuse refractives
	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const
	{
		wi = sampler.cosSampleHemisphereSurface();
		pdf = wi.z/PI;
		if (wo.z>0) wi.z *= -1;
		return f(wo, wi);
	}
};



class DiracBxDF : virtual public BxDF
{
public:
	DiracBxDF()
	{
		flags = DIRAC;
	}
	Color f(const vec3f& wo, const vec3f& wi) const final
	{
		return Color(0);
	}
};
