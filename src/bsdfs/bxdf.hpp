#pragma once

#include "geometry/geometry.hpp"
#include "color.h"
// BxDF should work on both hemispheres


class BxDF
{
public:
	enum Type {
		DIRAC = 1, // whether light sampling is invalid (f always returning 0)
		REFLECT = 2, // whether acceptable with rays in same side of geometric surface
		TRANSMIT = 4 // whether acceptable with rays passing through geometric surface
	};
	Type flags;
	bool isDirac() const { return flags & DIRAC; }
	bool isReflective() const { return flags & REFLECT; }
	bool isTransmissive() const { return flags & TRANSMIT; }
	// assuming normal N = (0,0,1)
	// return BxDF value (dL/dI) (except with Dirac)
	virtual Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const = 0;
	// return BxDF value, while providing a sample wi with pdf,
	// of which integral over surface of unit sphere is 1 (except with Dirac)
	virtual Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler&) const = 0;
};


class BRDF : virtual public BxDF
{
public:
	BRDF()
	{
		flags = REFLECT;
	}
	// default implementation for diffuse reflectives
	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const
	{
		wi = sampler.cosSampleHemisphereSurface();
		pdf = wi.z/PI;
		if (wo.z<0) wi.z *= -1;
		return f(albedo, wo, wi);
	}
};


class BTDF : virtual public BxDF
{
public:
	BTDF()
	{
		flags = TRANSMIT;
	}
	// default implementation for diffuse transmissives
	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const
	{
		wi = sampler.cosSampleHemisphereSurface();
		pdf = wi.z/PI;
		if (wo.z>0) wi.z *= -1;
		return f(albedo, wo, wi);
	}
};


class DiracBxDF : virtual public BxDF
{
public:
	DiracBxDF()
	{
		flags = DIRAC;
	}
	// for Dirac, f always return 0. Instead, bounces to light sources should not be discarded.
	Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const final
	{
		return Color(0);
	}
	// for Dirac, sample_f returns dI/dI, pdf is discrete probability
};
