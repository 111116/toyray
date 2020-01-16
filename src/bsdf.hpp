#pragma once

#include "jsonutil.hpp"

class BsDF {
public:
	Color albedo;
	BsDF(const Json& conf) {
		assert(conf["type"] == "lambert" || conf["type"] == "null");
		if (is_number(conf["albedo"])) {
			float t = conf["albedo"];
			albedo = Color(t,t,t);
		}
		else {
			albedo = json2vec3(conf["albedo"]);
		}
	}
	// wo, wi, N: outward unit vector
	Color f(vec3 wo, vec3 wi, vec3 N) {
		float t1 = dot(wo, N);
		float t2 = dot(wi, N);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) return Color();
		return 1/PI * albedo;
	}
	Color sample_f(vec3 wo, vec3& wi, vec3 N, float& pdf) {
		vec3 N1 = cross(N,vec3(0,0,1));
		if (norm(N1)<0.1) N1 = cross(N,vec3(0,1,0));
		N1 = normalize(N1);
		vec3 N2 = cross(N,N1);
		// orthonormal basis
		float x,y;
		do {
			x = randf()*2-1;
			y = randf()*2-1;
		}
		while (x*x+y*y>1);
		wi = x*N1 + y*N2 + std::sqrt(std::max(0.0f, 1-x*x-y*y))*N;
		float t1 = dot(wo, N);
		float t2 = dot(wi, N);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) wi = -wi;
		pdf = fabs(t2)/PI;
		return 1/PI * albedo;
	}
};