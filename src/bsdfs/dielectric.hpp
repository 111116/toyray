#pragma once

#include "bxdf.hpp"


class DielectricBRDF : public DiracBxDF
{
private:
	Color albedo;
	float IOR;
	
public:
	DielectricBRDF(float IOR, Color albedo): IOR(IOR), albedo(albedo)
	{
		flags = Type(DIRAC | TRANSMIT | REFLECT);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = vec3f(-wo.x, -wo.y, wo.z);
		float ior = wo.z>0? IOR: 1/IOR;
		float sini = sqrtf(wo.x*wo.x+wo.y*wo.y);
		float sint = sini / ior;
		if (fabs(sint) >= 1) { // total reflect
			wi = vec3f(-wo.x, -wo.y, wo.z);
			pdf = 1;
			return 1 / fabs(wi.z);
		}
		float cosi = fabs(wo.z);
		float cost = sqrtf(1-sint*sint);
		float r1 = (ior*cosi - cost) / (ior*cosi + cost);
		float r2 = (cosi - ior*cost) / (cosi + ior*cost);
		float r = 0.5 * (r2*r2 + r1*r1); // reflect ratio
		if (sampler.get1f()<r) {
			wi = vec3f(-wo.x, -wo.y, wo.z);
			pdf = r;
			return r / fabs(wi.z);
		}
		else {
			wi = vec3f(-wo.x/ior, -wo.y/ior, -wo.z*cost/cosi);
			pdf = 1-r;
			return (1-r) / (fabs(wi.z) * ior * ior);
			// the 1/ior^2 coefficient is used to convert energy to radiance.
			// it might not make a difference if light sources and camera are in media of the same IOR
		}
	}
};