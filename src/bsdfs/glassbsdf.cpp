
#include "bxdf.hpp"
#include "glassbsdf.hpp"

Color GlassBSDF::sample_f(const vec3f& wo, vec3f& wi, const vec3f& Ns, const vec3f& Ng, bool& isDirac, Sampler& sampler) const
{
	isDirac = true;
	vec3f N1,N2;
	getLocalBasis(Ns, N1, N2);
	vec3f wiL, woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec2f woLL(sqrtf(woL.x * woL.x + woL.y * woL.y), woL.z);
	float ior = woL.z>0? IOR: 1/IOR;
	float sini = woLL.x;
	float sint = sini / ior;
	if (sint >= 1) {
		wiL = vec3f(-woL.x, -woL.y, woL.z);
		wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
		return 1/fabs(woL.z);
	}
	float cosi = fabs(woL.z);
	float cost = sqrtf(1-sint*sint);
	float r1 = (ior*cosi - cost) / (ior*cosi + cost);
	float r2 = (cosi - ior*cost) / (cosi + ior*cost);
	float rR = 0.5 * (r2*r2 + r1*r1);
	float rT = 1 - rR;
	float f;
	if (sampler.get1f()<rR) {
		wiL = vec3f(-woL.x, -woL.y, woL.z);
		f = 1 / fabs(wiL.z);
	}
	else {
		wiL = vec3f(-woL.x/ior, -woL.y/ior, -woL.z*cost/cosi);
		f = 1 / fabs(wiL.z) / ior;
		// the 1/ior coefficient is used to convert energy to radiance.
		// it might not make a difference if light sources and camera are in media of the same IOR
	}
	wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
	return f;
	// don't check for now?
	// recalculate to make sure
	// Ng is used to determine whether BRDF or BTDF is used
	// bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	// Color val = (through ^! component->isRefractive)? f0: 0;
}

