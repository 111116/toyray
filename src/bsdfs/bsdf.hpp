#pragma once

#include "color.h"
#include "image.hpp"
#include "scalarimage.hpp"
#include "accelarator/accelarator.hpp"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp"

class Sampler;

class BSDF
{
	Image* albedoTexture = NULL;
	ScalarImage* bumpTexture = NULL;
	Color albedoConst = Color(1);

public:
	Color albedo(const vec2f& uv) const
	{
		return albedoTexture? albedoTexture->sample(vec2f(uv.x,-uv.y)): albedoConst;
	}

	BSDF(const Json& conf)
	{
		if (conf.find("Kd") != conf.end())
		{
			if (conf["Kd"].type() == Json::value_t::string)
				albedoTexture = new Image(getpath(conf["Kd"]));
			else
				albedoConst = json2vec3f(conf["Kd"]);
		}
		if (conf.find("albedo") != conf.end())
		{
			if (conf["albedo"].type() == Json::value_t::string)
				albedoTexture = new Image(getpath(conf["albedo"]));
			else
				albedoConst = json2vec3f(conf["albedo"]);
		}
		if (conf.find("bump") != conf.end())
		{
			bumpTexture = new ScalarImage(getpath(conf["bump"]));
		}
		if (conf.find("bumpmap") != conf.end())
		{
			bumpTexture = new ScalarImage(getpath(conf["bumpmap"]));
		}
	}

	void bump(HitInfo& hit)
	{
		if (bumpTexture == NULL) return;
		vec2f dhduv = bumpTexture->dfduv(vec2f(hit.uv.x,1-hit.uv.y));
		vec3f N = normalized(cross(hit.dpdu + hit.Ns * dhduv.x, hit.dpdv - hit.Ns * dhduv.y));
		if (dot(N, hit.Ns)<0) N = -N;
		hit.Ns = N;
	}
	// bidirectional scattering distribution function value of non-Dirac components
	// wo: direction of outgoing ray (normalized)
	// wi: directoin of incoming ray (inverted, normalized)
	// Ns: direction of shading normal vector (outward, normalized)
	// Ng: direction of geometry normal vector (outward, normalized)
	// Note that Ng will be used to determine whether BRDF or BTDF component is used,
	// while Ns is passed to BRDF/BTDF which works on both sides
	virtual Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const = 0;

	// bidirectional scattering distribution function value with importance sampling
	// integrator without MIS should be unaware of pdf
	// so for now we scale sampled f with 1/pdf internally
	virtual Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler&) const = 0;

protected:
	static void getLocalBasis(const vec3f& Ns, vec3f& N1, vec3f& N2)
	{
		// generate orthonormal basis [Ns,N1,N2]
		N1 = cross(Ns,vec3f(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3f(0,1,0));
		N1 = normalized(N1);
		N2 = cross(Ns,N1);
	}
};
