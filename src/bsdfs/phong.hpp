#pragma once

#include "bxdf.hpp"
#include "util/jsonutil.hpp"


class Phong : public BRDF
{
private:
	Color Ka, Ks;
	double exp = 0;
public:
	Phong(const Json& conf)
	{
		flags = REFLECT;
		if (conf.find("Ka") != conf.end()) Ka = json2vec3f(conf["Ka"]);
		if (conf.find("Ks") != conf.end()) Ks = json2vec3f(conf["Ks"]);
		if (conf.find("exp") != conf.end()) exp = conf["exp"];
	}
	Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const override
	{
		float spec = std::max(0.0f, dot(vec3f(-wi.x, -wi.y, wi.z), wo));
		return 1/PI * (albedo + 1/fabs(wi.z) * Ks * pow(spec, exp));
	}
};