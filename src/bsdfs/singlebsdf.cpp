
#include "bxdf.hpp"
#include "singlebsdf.hpp"


Color SingleBSDF::f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
{
	if (component == NULL) return Color(0);
	vec3f N1,N2;
	getLocalBasis(hit.Ns, N1, N2);
	// transform wo, wi to local surface Ns' = (0,0,1)
	vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,hit.Ns));
	vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,hit.Ns));
	// use Ng to determine whether BRDF or BTDF is used
	bool through = (dot(wo, hit.Ng)>0) ^ (dot(wi, hit.Ng)>0);
	BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
	return (component->flags & allowed)? component->f(albedo(hit.uv), woL, wiL): 0;
}

Color SingleBSDF::sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
{
	if (component == NULL)
	{
		wi = vec3f(0,1,0);
		return 0;
	}
	// generate sample
	vec3f N1,N2;
	getLocalBasis(hit.Ns, N1, N2);
	vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,hit.Ns));
	float pdf;
	Color f0 = component->sample_f(albedo(hit.uv), woL, wiL, pdf, sampler);
	wi = wiL.x * N1 + wiL.y * N2 + wiL.z * hit.Ns;
	// recalculate to make sure
	// Ng is used to determine whether BRDF or BTDF is used
	bool through = (dot(wo, hit.Ng)>0) ^ (dot(wi, hit.Ng)>0);
	BxDF::Type allowed = through? BxDF::TRANSMIT : BxDF::REFLECT;
	Color val = (component->flags & allowed)? f0: 0;
	isDirac = (component->flags & BxDF::DIRAC);
	return val / pdf;
}

