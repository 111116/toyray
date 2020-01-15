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
	// wo, wi, N: outward unit vector
	vec3 f(vec3 wo, vec3 wi, vec3 N) {
		float t1 = dot(wo, N);
		float t2 = dot(wi, N);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) return vec3();
		return 1/PI * albedo;
	}
	vec3 sample_f(vec3 wo, vec3& wi, vec3 N, float& pdf) {
		// vec3 v;
		// while (norm(v) < 1e-3)
		// {
		// 	do v = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
		// 	while (norm(v)>1 || norm(v)<1e-3);
		// 	v = normalize(v) + N;
		// }
		// wi = normalize(v);
		// pdf = 
		wi = randunitvec3();
		float t1 = dot(wo, N);
		float t2 = dot(wi, N);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) wi = -wi;
		pdf = 0.5/PI;
		return 1/PI * albedo;
	}
};