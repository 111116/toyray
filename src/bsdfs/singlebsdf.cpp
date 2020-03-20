
#include "bxdf.hpp"
#include "singlebsdf.hpp"


Color SingleBSDF::f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
{
	if (component == NULL) return Color(0);
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	// transform wo, wi to local surface Ns' = (0,0,1)
	vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
	// use Ng to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	Color val(0);
	if (through ^! component->isRefractive)
		val += component->f(woL, wiL);
	return val;
}

Color SingleBSDF::sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler& sampler) const
{
	if (component == NULL)
	{
		wi = vec3f(0,1,0);
		return 0;
	}
	// generate sample
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	float pdf;
	Color f0 = component->sample_f(woL, wiL, pdf, sampler);
	wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
	// return this single result if dirac
	if (component->isDirac)
	{
		isDirac = true;
		return f0 / pdf;
	}
	// recalculate to make sure
	// Ng is used to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	Color val = (through ^! component->isRefractive)? f0: 0;
	isDirac = false;
	return val / pdf;
}

