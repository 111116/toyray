#pragma once

#include "bxdf.hpp"
#include "../jsonutil.hpp"


class BSDF {
public:
	BxDF* component = NULL;
	// bidirectional scattering distribution function value of non-delta components
	// wo: direction of outgoing ray (normalized)
	// wi: direction of incoming ray (inverted, normalized)
	// Ns: direction of shading normal vector (outward, normalized)
	// Ng: direction of geometry normal vector (outward, normalized)
	// Note that Ng will be used to determine whether BRDF or BTDF component is used,
	// while Ns is passed to BRDF/BTDF which works continuously on both sides
	Color f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const;
};


BSDF* newMaterial(const Json& conf) {
	BSDF* bsdf = new BSDF();
	if (conf["type"] == "phong")
		bsdf->component = new Phong(conf);
	if (conf["type"] == "lambert")
		bsdf->component = new LambertBRDF(json2vec3f(conf["albedo"]));
	return bsdf;
}


Color BSDF::f(const vec3f& wo, const vec3f& wi, const vec3f& Ns, const vec3f& Ng) const
{
	if (component == NULL) return Color(0);
	// generate orthonormal basis [Ns,N1,N2]
	vec3f N1 = cross(Ns,vec3f(0,0,1));
	if (norm(N1)<0.1) N1 = cross(Ns,vec3f(0,1,0));
	N1 = normalized(N1);
	vec3f N2 = cross(Ns,N1);
	// transform wo, wi to local surface Ns = (0,0,1)
	vec3f woL = vec3f(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3f wiL = vec3f(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
	// use Ng to determine which BxDF to use
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng)>0);
	if (through) return Color(0);
	return component->f(woL, wiL);
}

