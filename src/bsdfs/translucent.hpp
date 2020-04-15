#pragma once

#include "color.h"
#include "bsdf.hpp"
#include "lambert.hpp"
#include "invisible.hpp"


class Translucent : public BSDF
{
private:
	BSDF const* componentD = NULL;
	BSDF const* componentS = NULL;
	// amount of specular transmission
	float Ks;

public:
	Translucent(const Json& conf): BSDF(conf)
	{
		componentD = new SingleBSDF(conf, new LambertBTDF());
		// specular component
		auto conf1 = conf;
		conf1["Kd"] = 1;
		componentS = new SingleBSDF(conf1, new InvisibleBTDF());
		vec3f Ks = 0.25;
		if (conf.find("Ks") != conf.end()) {
			Ks = json2vec3f(conf["Ks"]);
		}
		if (Ks.x != Ks.y || Ks.x != Ks.z) throw "invalid specular amount";
		this->Ks = Ks.x;
	}

	Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
	{
		return componentD->f(wo,wi,hit);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
	{
		float p = 0.5;
		return sampler.get1f() < Ks?
			componentS->sample_f(wo,wi,hit,isDirac,sampler) * (Ks/p):
			componentD->sample_f(wo,wi,hit,isDirac,sampler) * (1/(1-p));
	}

};
