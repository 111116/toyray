#pragma once

#include "bxdf.hpp"
#include "microfacet.hpp"
#include "util/jsonutil.hpp"
#include "samplers/mt19937sampler.hpp"


// plastic model: specular dielectric layered lambertian surface
class RoughPlasticBSDF : public BSDF
{
private:
	Color albedo;
	float IOR;
	float outRatio;
	float alpha;
	
	Color f(const vec3f& wo, const vec3f& wi) const
	{
		float energy = 1;
		energy *= 1 - DielectricBRDF::reflectivity(fabs(wo.z), IOR);
		float sint = sqrtf(1-wi.z*wi.z)/IOR;
		if (sint >= 1) return 0; // total reflect (shouldn't happen if IOR>1)
		float cost = sqrtf(1-sint*sint);
		energy *= 1 - DielectricBRDF::reflectivity(cost, 1/IOR);
		float ratio2 = 1/(IOR*IOR); // see note_refract_radiance
		vec3f diff = energy / (1 - albedo * (1-outRatio)) * ratio2 * 1/PI * albedo;
		// add reflect component
		vec3f wm = normalized(wo+wi); // microfacet normal
		float cos = dot(wm,wo);
		Color Fres = DielectricBRDF::reflectivity(cos, IOR); // reflected energy
		vec3f refl = EDX::GGX_D(wm,alpha) * EDX::Smith_G(wo,wi,wm,alpha) / (4 * dot(wo,wm) * dot(wi,wm)) * Fres;
		// console.log(refl, diff);
		return refl + diff;
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, bool& isDirac, Sampler& sampler) const
	{
		isDirac = false;
		wi = sampler.cosSampleHemisphereSurface();
		pdf = wi.z/PI;
		if (wo.z<0) wi.z *= -1;
		return f(wo, wi);
	}

public:
	RoughPlasticBSDF(float IOR, Color albedo, float alpha):
		IOR(IOR), albedo(albedo), alpha(alpha)
	{
		// precompute outRatio
		// here bruteforcing to demonstrate its physical meaning
		// you can optimize by using approximating polynormial
		MT19937Sampler sampler;
		const int nsample = 1000000;
		double energy = 0; // use double here because accumulating process introduces lots of rounding error
		for (int i=0; i<nsample; ++i) {
			vec3f lambert_out = sampler.cosSampleHemisphereSurface();
			energy += 1 - DielectricBRDF::reflectivity(lambert_out.z, 1/IOR);
		}
		outRatio = energy / nsample;
	}

	// wrapper function f
	Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
	{
		vec3f N1,N2;
		getLocalBasis(Ns, N1, N2);
		// transform wo, wi to local surface Ns' = (0,0,1)
		vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
		vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
		// use Ng to determine whether BRDF or BTDF is used
		bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
		BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
		return (BxDF::REFLECT & allowed)? f(woL, wiL): 0;
	}

	// wrapper function sample_f
	Color sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler& sampler) const
	{
		// generate sample
		vec3f N1,N2;
		getLocalBasis(Ns, N1, N2);
		vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
		float pdf;
		Color f0 = sample_f(woL, wiL, pdf, isDirac, sampler);
		wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
		// recalculate to make sure
		// Ng is used to determine whether BRDF or BTDF is used
		bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
		BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
		Color val = (BxDF::REFLECT & allowed)? f0: 0;
		return val / pdf;
	}
};


