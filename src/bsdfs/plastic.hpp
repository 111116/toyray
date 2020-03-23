#pragma once

#include "bxdf.hpp"
#include "microfacet.hpp"
#include "util/jsonutil.hpp"
#include "samplers/mt19937sampler.hpp"


// plastic model: specular dielectric layered lambertian surface
class PlasticBSDF : public BSDF
{
private:
	float IOR;
	float outRatio;
	// portion of transmitted energy after first lambertian bounce
	// is used to scale non-Dirac component to account for multiple inter-reflections
	// i.e. (1-outRatio)*albedo is rescattered every time
	
	Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const
	{
		// non-Dirac component only
		float energy = 1;
		energy *= 1 - DielectricBRDF::reflectivity(fabs(wo.z), IOR);
		float sint = sqrtf(1-wi.z*wi.z)/IOR;
		if (sint >= 1) return 0; // total reflect (shouldn't happen if IOR>1)
		float cost = sqrtf(1-sint*sint);
		energy *= 1 - DielectricBRDF::reflectivity(cost, 1/IOR);
		float ratio2 = 1/(IOR*IOR); // see note_refract_radiance
		return energy / (1 - albedo * (1-outRatio)) * ratio2 * 1/PI * albedo;
	}

	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, bool& isDirac, Sampler& sampler) const
	{
		float r = DielectricBRDF::reflectivity(fabs(wo.z), IOR);
		// specular reflection
		if (sampler.get1f() < r) {
			pdf = r;
			isDirac = true;
			wi = vec3f(-wo.x, -wo.y, wo.z);
			return r / fabs(wi.z);
		}
		// diffuse reflection
		isDirac = false;
		// wi from inner surface
		wi = sampler.cosSampleHemisphereSurface();
		pdf = (1-r) * wi.z / PI;
		if (wo.z<0) wi.z *= -1;
		return f(albedo, wo, wi);
		// wi = sampler.cosSampleHemisphereSurface();
		// pdf = wi.z/PI;
		// if (wo.z<0) wi.z *= -1;
		// return f(wo, wi);
		// // diffuse component only !!!!!!!
	}

public:
	PlasticBSDF(const Json& conf): BSDF(conf)
	{
		IOR = conf["ior"];
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
	Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
	{
		vec3f N1,N2;
		getLocalBasis(hit.Ns, N1, N2);
		// transform wo, wi to local surface Ns' = (0,0,1)
		vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,hit.Ns));
		vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,hit.Ns));
		// use Ng to determine whether BRDF or BTDF is used
		bool through = (dot(wo, hit.Ng)>0) ^ (dot(wi, hit.Ng)>0);
		BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
		return (BxDF::REFLECT & allowed)? f(albedo(hit.uv), woL, wiL): 0;
	}

	// wrapper function sample_f
	Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
	{
		// generate sample
		vec3f N1,N2;
		getLocalBasis(hit.Ns, N1, N2);
		vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,hit.Ns));
		float pdf;
		Color f0 = sample_f(albedo(hit.uv), woL, wiL, pdf, isDirac, sampler);
		wi = wiL.x * N1 + wiL.y * N2 + wiL.z * hit.Ns;
		// recalculate to make sure
		// Ng is used to determine whether BRDF or BTDF is used
		bool through = (dot(wo, hit.Ng)>0) ^ (dot(wi, hit.Ng)>0);
		BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
		Color val = (BxDF::REFLECT & allowed)? f0: 0;
		return val / pdf;
	}
};


