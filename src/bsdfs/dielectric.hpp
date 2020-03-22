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

	// Fresnel's law for dielectrics; cosi>0
	static inline float reflectivity(float cosi, float ior)
	{
		float sini = sqrtf(1-cosi*cosi);
		float sint = sini / ior;
		if (sint >= 1) return 1; // total reflect
		float cost = sqrtf(1-sint*sint);
		float r1 = (ior*cosi - cost) / (ior*cosi + cost);
		float r2 = (cosi - ior*cost) / (cosi + ior*cost);
		return 0.5 * (r2*r2 + r1*r1);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		float ior = wo.z>0? IOR: 1/IOR;
		float r = reflectivity(fabs(wo.z), ior);

		if (r == 1 || sampler.get1f()<r) // reflected
		{
			wi = vec3f(-wo.x, -wo.y, wo.z);
			pdf = r;
			return r / fabs(wi.z);
		}
		else // transmitted
		{
			wi = vec3f(-wo.x/ior, -wo.y/ior, 0);
			wi.z = sqrtf(fmax(0, 1 - wi.x*wi.x - wi.y*wi.y));
			if (wo.z > 0) wi.z *= -1;
			pdf = 1-r;
			return (1-r) / (fabs(wi.z) * ior * ior);
			// the 1/ior^2 coefficient is used to convert energy to radiance
			// see note_refract_radiance
		}
	}
};