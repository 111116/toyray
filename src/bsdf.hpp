#pragma once

#include "bxdf.hpp"
#include "jsonutil.hpp"

class BSDF {
protected:
	BxDF* reflect = NULL;
	BxDF* refract = NULL;
public:
	virtual Color f(const vec3& wo, const vec3& wi, const vec3& Ns, const vec3& Ng) const {
		// generate orthonormal basis [Ns,N1,N2]
		vec3 N1 = cross(Ns,vec3(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3(0,1,0));
		N1 = normalize(N1);
		vec3 N2 = cross(Ns,N1);
		// transform wo, wi to local surface Ns = (0,0,1)
		vec3 woL = vec3(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
		vec3 wiL = vec3(dot(wi,N1), dot(wi,N2), dot(wi,Ns));
		// use Ng to determine which BxDF to use
		float sgno = dot(wo, Ng);
		float sgni = dot(wi, Ng);
		// ignore edge case
		if (sgno==0 || sgni==0) return Color(0);
		bool through = (sgno>0) ^ (sgni>0);
		if (!through && reflect) return reflect->f(woL, wiL);
		if ( through && refract) return refract->f(woL, wiL);
		return Color(0);
	}
	virtual Color sample_f(const vec3& wo, vec3& wi, const vec3& Ns, const vec3& Ng, float& pdf) const {
		// choose which BxDF to use
		BxDF* bxdf = reflect? (refract? (rand()%2? reflect: refract): reflect): refract;
		if (bxdf == NULL) {
			wi = vec3(0,1,0);
			pdf = 1;
			return Color(0);
		}
		// generate orthonormal basis [Ns,N1,N2]
		vec3 N1 = cross(Ns,vec3(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3(0,1,0));
		N1 = normalize(N1);
		vec3 N2 = cross(Ns,N1);
		// transform wo to local surface Ns = (0,0,1)
		vec3 woL = vec3(dot(wo,N1), dot(wo,N2), dot(wo,Ns));
		vec3 wiL;
		Color f = bxdf->sample_f(woL, wiL, pdf);
		wi = wiL.x * N1 + wiL.y * N2 + wiL.z * Ns;
		float sgno = dot(wo, Ng);
		float sgni = dot(wi, Ng);
		if (sgno==0 || sgni==0) return Color(0);
		bool through = (sgno>0) ^ (sgni>0);
		if (!through && bxdf!=reflect) return Color(0);
		if ( through && bxdf!=refract) return Color(0);
		return f;
	}
};


class LambertReflect : public BSDF
{
public:
	LambertReflect(const Json& conf) {
		assert(conf["type"] == "lambert");
		reflect = new LambertBRDF(json2vec3(conf["albedo"]));
	}
};


// class Conductor : public BSDF
// {
// public:
// 	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const = 0;
// 	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const = 0;
// };


// class Dielectric : public BSDF
// {
// public:
// 	Color f(vec3 wo, vec3 wi, vec3 Ns, vec3 Ng) const
// 	{

// 	}
// 	Color sample_f(vec3 wo, vec3& wi, vec3 Ns, vec3 Ng, float& pdf) const
// 	{

// 	}
// };
