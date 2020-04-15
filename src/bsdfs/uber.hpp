#pragma once

#include "color.h"
#include "bsdf.hpp"
#include "rough_plastic.hpp"
#include "mirror.hpp"


// trash can bsdf
class Uber : public BSDF
{
private:
	BSDF const* componentD = NULL;
	BSDF const* componentS = NULL;
	// amount of componentA
	float Kr;
	float ior;

	// remap roughness because this is pbrt material
	float RoughnessToAlpha(float roughness) {
		float x = std::log(roughness);
		return 1.62142f + 0.819955f * x + 0.1734f * x * x +
			0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
	}

public:
	Uber(const Json& conf): BSDF(conf)
	{
		// rough component
		float rough = 0.1;
		if (conf.find("roughness") != conf.end())
			rough = conf["roughness"];
		auto conf_plastic = conf;
		conf_plastic["roughness"] = RoughnessToAlpha(rough);
		auto plastic = new RoughPlasticBSDF(conf_plastic);
		vec3f Ks = 0.25;
		if (conf.find("Ks") != conf.end()) {
			try {
				Ks = json2vec3f(conf["Ks"]);
			}
			catch (const char* err) {
				console.warn(err);
				Ks = 0;
			}
		}
		if (Ks.x != Ks.y || Ks.x != Ks.z) throw "invalid glossy amount";
		plastic->Ks = Ks.x;
		componentD = plastic;
		// mirror component
		auto conf_mirror = conf;
		conf_mirror["Kd"] = 1;
		componentS = new SingleBSDF(conf_mirror, new MirrorBRDF());
		vec3f Kr = 0;
		if (conf.find("Kr") != conf.end()) {
			try {
				Kr = json2vec3f(conf["Kr"]);
			}
			catch (const char* err) {
				console.warn(err);
				Kr = 0;
			}
		}
		if (Kr.x != Kr.y || Kr.x != Kr.z) throw "invalid specular amount";
		this->Kr = Kr.x;
		this->ior = 1.5;
		if (conf.find("ior") != conf.end())
			this->ior = conf["ior"];
	}

	Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
	{
		return componentD->f(wo,wi,hit);
	}

	Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
	{
		return sampler.get1f() < Kr?
			componentS->sample_f(wo,wi,hit,isDirac,sampler) * DielectricBRDF::reflectivity(fabs(wo.z),ior):
			componentD->sample_f(wo,wi,hit,isDirac,sampler) * (1/(1-Kr));
	}

};
