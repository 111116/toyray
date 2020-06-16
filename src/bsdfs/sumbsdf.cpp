
#include "sumbsdf.hpp"


Color SumBSDF::f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
{
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	// transform wo, wi to local surface Ns' = (0,0,1)
	vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
	// use Ng to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
	Color val(0);
	for (int i=0; i<n_component; ++i)
		if (components[i]->flags & allowed)
			val += components[i]->f(woL, wiL);
	return val;
}

Color SumBSDF::sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler& sampler) const
{
	if (n_component == 0)
	{
		wi = vec3f(0,1,0);
		return 0;
	}
	// select sampled component (with equal probability for now)
	unsigned id = sampler.get1u(n_component);
	// generate sample
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	float pdf;
	Color f0 = components[id]->sample_f(woL, wiL, pdf, sampler);
	pdf /= n_component;
	wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
	// return this single result if dirac
	if (components[id]->isDirac())
	{
		isDirac = true;
		return f0 / pdf;
	}
	// discard f0 and recalculate to make sure
	// Ng is used to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
	int n_sampled = 0;
	Color val(0);
	for (int i=0; i<n_component; ++i)
		if (!components[i]->isDirac() && (components[i]->flags & allowed))
		{
			n_sampled += 1;
			val += components[i]->f(woL, wiL);
		}
	pdf *= n_sampled;
	isDirac = false;
	return val / pdf;
}

