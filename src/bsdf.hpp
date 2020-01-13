#pragma once

#include "jsonutil.hpp"

class BsDF {
public:
	vec3 albedo;
	BsDF(const Json& conf) {
		assert(conf["type"] == "lambert" || conf["type"] == "null");
		if (is_number(conf["albedo"])) {
			float t = conf["albedo"];
			albedo = vec3(t,t,t);
		}
		else {
			albedo = json2vec3(conf["albedo"]);
		}
	}
	vec3 fr(vec3 wo, vec3 wi, vec3 N) {
		float t1 = dot(wo, N);
		float t2 = dot(wi, N);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) return vec3();
		return 1/PI * albedo;
	}
};