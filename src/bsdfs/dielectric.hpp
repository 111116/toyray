#pragma once

#include "bxdf.hpp"


class DielectricBRDF : public BRDF, public DiracBxDF
{
private:
	Color albedo;
	float IOR;
	
public:
	DielectricBRDF(float IOR, Color albedo): IOR(IOR), albedo(albedo) {}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = vec3f(-wo.x, -wo.y, wo.z);
		pdf = 1;
		float ior = wo.z>0? IOR: 1/IOR;
		float sini = sqrtf(wo.x*wo.x+wo.y*wo.y);
		float sint = sini / ior;
		if (fabs(sint) >= 1) return 1 / fabs(wo.z) * albedo;
		float cosi = fabs(wo.z);
		float cost = sqrtf(1-sint*sint);
		float r1 = (ior*cosi - cost) / (ior*cosi + cost);
		float r2 = (cosi - ior*cost) / (cosi + ior*cost);
		return 0.5 / fabs(wo.z) * (r2*r2 + r1*r1) * albedo;
	}
};

class DielectricBTDF : public BTDF, public DiracBxDF
{
private:
	Color albedo;
	float IOR;
	
public:
	DielectricBTDF(float IOR, Color albedo): IOR(IOR), albedo(albedo) {}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		pdf = 1;
		float ior = wo.z>0? IOR: 1/IOR;
		float sini = sqrtf(wo.x*wo.x+wo.y*wo.y);
		float sint = sini / ior;
		if (fabs(sint) >= 1) return 0;
		float cosi = fabs(wo.z);
		float cost = sqrtf(1-sint*sint);
		wi = vec3f(-wo.x / ior, -wo.y / ior, -wo.z * cost/cosi);
		float r1 = (ior*cosi - cost) / (ior*cosi + cost);
		float r2 = (cosi - ior*cost) / (cosi + ior*cost);
		return 0.5 / fabs(wi.z) * (2 - r2*r2 - r1*r2) * albedo;
	}
};