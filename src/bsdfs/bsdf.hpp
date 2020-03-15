#pragma once

#include "bxdf.hpp"
#include "../jsonutil.hpp"


class BSDF
{
private:
	static const int MAXN_COMPONENT = 4;
	int n_component = 0;
	BxDF const* components[MAXN_COMPONENT];

public:
	void add_component(BxDF const* component) {
		if (n_component >= MAXN_COMPONENT)
			throw "BSDF too many components";
		components[n_component++] = component;
	}
	bool empty() const {
		return n_component == 0;
	}
	// bidirectional scattering distribution function value of non-Dirac components
	// wo: direction of outgoing ray (normalized)
	// wi: directoin of incoming ray (inverted, normalized)
	// Ns: direction of shading normal vector (outward, normalized)
	// Ng: direction of geometry normal vector (outward, normalized)
	// Note that Ng will be used to determine whether BRDF or BTDF component is used,
	// while Ns is passed to BRDF/BTDF which works on both sides
	Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const;

	// bidirectional scattering distribution function value with importance sampling
	// integrator without MIS should be unaware of pdf
	// so for now we scale sampled f with 1/pdf internally
	Color sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler&) const;

private:
	static void getLocalBasis(const vec3f& Ns, vec3f& N1, vec3f& N2)
	{
		// generate orthonormal basis [Ns,N1,N2]
		N1 = cross(Ns,vec3f(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3f(0,1,0));
		N1 = normalized(N1);
		N2 = cross(Ns,N1);
	}
};

Color BSDF::f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
{
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	// transform wo, wi to local surface Ns' = (0,0,1)
	vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
	// use Ng to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	Color val(0);
	for (int i=0; i<n_component; ++i)
		if (through ^! components[i]->isRefractive)
			val += components[i]->f(woL, wiL);
	return val;
}

Color BSDF::sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler& sampler) const
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
	if (components[id]->isDirac)
	{
		isDirac = true;
		return f0 / pdf;
	}
	// discard f0 and recalculate to make sure
	// Ng is used to determine whether BRDF or BTDF is used
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	int n_sampled = 0;
	Color val(0);
	for (int i=0; i<n_component; ++i)
		if (!components[i]->isDirac && (through ^! components[i]->isRefractive))
		{
			n_sampled += 1;
			val += components[i]->f(woL, wiL);
		}
	pdf *= n_sampled;
	isDirac = false;
	return val / pdf;
}

