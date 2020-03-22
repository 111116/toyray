#pragma once

#include "bxdf.hpp"
#include "../lib/json.hpp"


class Phong : public BRDF
{
private:
	Color Ka, Kd, Ks;
	double exp = 0;
public:
	Phong(const Json& conf)
	{
		flags = REFLECT;
		if (conf.find("Ka") != conf.end()) Ka = json2vec3f(conf["Ka"]);
		if (conf.find("Kd") != conf.end()) Kd = json2vec3f(conf["Kd"]);
		if (conf.find("Ks") != conf.end()) Ks = json2vec3f(conf["Ks"]);
		if (conf.find("exp") != conf.end()) exp = conf["exp"];
	}
	Color f(const vec3f& wo, const vec3f& wi) const
	{
		float spec = std::max(0.0f, dot(vec3f(-wi.x, -wi.y, wi.z), wo));
		return 1/PI * (Kd + 1/fabs(wi.z) * Ks * pow(spec, exp));
	}
};