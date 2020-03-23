#pragma once

#include "color.h"
#include "bsdf.hpp"

class BxDF;

// wrapper of low-level BxDFs
class SingleBSDF : public BSDF
{
private:
	BxDF const* component = NULL;

public:
	SingleBSDF(const Json& conf, BxDF const* component): BSDF(conf), component(component) {}
	Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const final;
	Color sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler&) const final;

};
