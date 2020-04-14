
#include "bxdf.hpp"
#include "twosidemix.hpp"


Color TwoSideMix::f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
{
	return amount * componentA->f(wo,wi,hit)
	 + (1-amount) * componentB->f(wo,wi,hit);
}

Color TwoSideMix::sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
{
	return sampler.get1f() < amount?
		componentA->sample_f(wo,wi,hit,isDirac,sampler):
		componentB->sample_f(wo,wi,hit,isDirac,sampler);
}

