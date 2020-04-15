#pragma once

#include "color.h"
#include "bsdf.hpp"


// mixture of non-overlapping BSDFs
class TwoSideMix : public BSDF
{
private:
	BSDF const* componentA = NULL;
	BSDF const* componentB = NULL;
	// amount of componentA
	float amount;

public:
	TwoSideMix(const Json& conf, BSDF const* componentA, BSDF const* componentB):
		BSDF(conf), componentA(componentA), componentB(componentB)
	{
		vec3f vamount = json2vec3f(conf["amount"]);
		if (vamount.x != vamount.y || vamount.x != vamount.z) throw "mix: invalid amount";
		amount = vamount.x;
		if (componentA == NULL || componentB == NULL) throw "mix: invalid components";
		if (!(amount>=0 && amount<1)) throw "mix: invalid amount";
	}
	Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const final;
	Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler&) const final;

};
