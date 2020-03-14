#pragma once

#include "../geometry/geometry.hpp"
#include "../color.h"

// BxDF should work on both hemispheres

class BxDF {
public:
	// assuming normal N = (0,0,1)
	virtual Color f(const vec3f& wo, const vec3f& wi) const = 0;
};


class Phong : public BxDF {
public:
	Color Ka, Kd, Ks;
	double exp = 0;
	Phong(const Json& conf) {
		if (conf.find("Ka") != conf.end()) Ka = json2vec3f(conf["Ka"]);
		if (conf.find("Kd") != conf.end()) Kd = json2vec3f(conf["Kd"]);
		if (conf.find("Ks") != conf.end()) Ks = json2vec3f(conf["Ks"]);
		if (conf.find("exp") != conf.end()) exp = conf["exp"];
	}
	Color f(const vec3f& wo, const vec3f& wi) const {
		return 1/PI * (Kd + 1/abs(wi.z) * Ks * pow(std::max(0.0f,dot(vec3f(-wi.x, -wi.y, wi.z), wo)), exp));
	}
};
