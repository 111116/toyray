#pragma once

#include "jsonutil.hpp"

class BSDF {
public:
	Color albedo;
	virtual Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const = 0;
	virtual Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const = 0;
};

class LambertReflect : public BSDF
{
public:
	LambertReflect(const Json& conf) {
		assert(conf["type"] == "lambert" || conf["type"] == "null");
		if (is_number(conf["albedo"])) {
			float t = conf["albedo"];
			albedo = Color(t,t,t);
		}
		else {
			albedo = json2vec3(conf["albedo"]);
		}
	}

	// wo, wi, Ns, Ng: outward unit vector
	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const {
		float t1 = dot(wo, Ng);
		float t2 = dot(wi, Ng);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) return Color();
		return 1/PI * albedo;
	}

	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const {
		vec3 N1 = cross(Ns,vec3(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3(0,1,0));
		N1 = normalize(N1);
		vec3 N2 = cross(Ns,N1);
		// orthonormal basis
		float x,y;
		do {
			x = randf()*2-1;
			y = randf()*2-1;
		}
		while (x*x+y*y>1);
		wi = x*N1 + y*N2 + std::sqrt(std::max(0.0f, 1-x*x-y*y))*Ns;
		float t1 = dot(wo, Ns);
		float t2 = dot(wi, Ns);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) wi = -wi;
		pdf = fabs(t2)/PI;

		t1 = dot(wo, Ng);
		t2 = dot(wi, Ng);
		if ((t1<0 && t2>0) || (t1>0 && t2<0)) return Color();
		return 1/PI * albedo;
	}
};


class Conductor : public BSDF
{
public:
	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const = 0;
	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const = 0;
};


class Dielectric : public BSDF
{
public:
	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const
	{

	}
	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const
	{

	}
};
