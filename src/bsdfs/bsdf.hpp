#pragma once

#include "color.h"
#include "image.hpp"
#include "accelarator/accelarator.hpp"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp"

class Sampler;

class BSDF
{
	Image* albedoTexture = NULL;
	Color albedoConst = Color(1);

protected:
	Color albedo(const vec2f& uv) const
	{
		return albedoTexture? albedoTexture->sample(uv): albedoConst;
	}

public:
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
