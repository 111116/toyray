#pragma once

#include "color.h"
#include "bxdf.hpp"
#include "bsdf.hpp"

class Sampler;

// wrapper of low-level BxDFs
class SumBSDF : public BSDF
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
	virtual Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const;

	// bidirectional scattering distribution function value with importance sampling
	// integrator without MIS should be unaware of pdf
	// so for now we scale sampled f with 1/pdf internally
	virtual Color sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler&) const;

};
