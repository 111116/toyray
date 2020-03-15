#pragma once

#include "bxdf.hpp"
#include "../jsonutil.hpp"


class BSDF
{
private:
	static const MAXN_COMPONENT = 4;
	int n_component = 0;
	BxDF const* components[MAXN_COMPONENT];

public:
	void add_component(BxDF const* component) {
		if (n_component >= MAXN_COMPONENT)
			throw "BSDF too many components";
		components[n_component++] = component;
	}
	// bidirectional scattering distribution function value of non-Dirac components
	// wo: direction of outgoing ray (normalized)
	// wi: directoin of incoming ray (inverted, normalized)
	// Ns: direction of shading normal vector (outward, normalized)
	// Ng: direction of geometry normal vector (outward, normalized)
	// Note that Ng will be used to determine whether BRDF or BTDF component is used,
	// while Ns is passed to BRDF/BTDF which works on both sides
	Color f(const vec3& wo, const vec3& wi, const vec3& Ns, const vec3& Ng) const;

	// bidirectional scattering distribution function value with importance sampling
	// integrator without MIS should be unaware of pdf
	// so for now we scale sampled f with 1/pdf internally
	Color sample_f(const vec3& wo, vec3& wi, const vec3& Ns, const vec3& Ng, bool& isDirac) const;

private:
	void getLocalBasis(const vec3f&)
};


BSDF* newMaterial(const Json& conf) {
	BSDF* bsdf = new BSDF();
	if (conf["type"] == "lambert")
		bsdf->add_component(new LambertBRDF(json2vec3(conf["albedo"])));
	if (conf["type"] == "mirror")
		bsdf->add_component(new MirrorBRDF(json2vec3(conf["albedo"])));
	if (conf["type"] == "invisible")
		bsdf->add_component(new InvisibleBTDF());
	return bsdf;
}


// class Conductor : public BSDF
// {
// public:
// 	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const = 0;
// 	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const = 0;
// };


// class Dielectric : public BSDF
// {
// public:
// 	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const
// 	{

// 	}
// 	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const
// 	{

// 	}
// };

Color BSDF::f(const vec3& wo, const vec3& wi, const vec3& Ns, const vec3& Ng, BxDFType allowed_type) const
{
	// generate orthonormal basis [Ns,N1,N2]
	vec3 N1 = cross(Ns,vec3(0,0,1));
	if (norm(N1)<0.1) N1 = cross(Ns,vec3(0,1,0));
	N1 = normalize(N1);
	vec3 N2 = cross(Ns,N1);
	// transform wo, wi to local surface Ns = (0,0,1)
	vec3 woL = vec3(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3 wiL = vec3(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
	// use Ng to determine which BxDF to use
	bool through = (dot(wo, Ng)>0) ^ (dot(wi, Ng>0));
	allowed_type |= (through? ALL_TRANSMIT: ALL_REFLECT);
	Color val(0);
	for (int i=0; i<n_component; ++i)
		if ((components[i]->type | allowed_type) == allowed_type)
			val += components[i]->f(woL, wiL);
	return val;
}

Color BSDF::sample_f(const vec3& wo, vec3& wi, const vec3& Ns, const vec3& Ng, float& pdf, BxDFType allowed_type, BxDFType& sampled_type) const
{
	// select lobe
	BxDF* selected[MAXN_COMPONENT];
	int n_selected = 0;
	for (int i=0; i<n_component; ++i)
		if ((components[i]->type | allowed_type) == allowed_type)
			selected[n_selected++] = components[i];
	// if selected is empty, f is zero
	if (n_selected == 0)
	// generate orthonormal basis [Ns,N1,N2]
	vec3 N1 = cross(Ns,vec3(0,0,1));
	if (norm(N1)<0.1) N1 = cross(Ns,vec3(0,1,0));
	N1 = normalize(N1);
	vec3 N2 = cross(Ns,N1);
	// transform wo to local surface Ns = (0,0,1)
	vec3 woL = vec3(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
	vec3 wiL;

}

