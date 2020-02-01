#pragma once

#include "../geometry/geometry.hpp"

// BxDF should work on both hemispheres,
// to allow difference between shading normal & geometry normal.

class BxDF {
public:
	// assuming normal N = (0,0,1)
	virtual Color f(const vec3& wo, const vec3& wi) const = 0;
	virtual Color sample_f(const vec3& wo, vec3& wi, float& pdf) const = 0;
};

class BRDF : public BxDF {
public:
	virtual Color sample_f(const vec3& wo, vec3& wi, float& pdf) const {
		// sample upper hemisphere with p = cos theta
		float x,y;
		do {
			x = randf()*2-1;
			y = randf()*2-1;
		}
		while (x*x+y*y>1);
		wi = vec3(x,y,std::sqrt(std::max(0.0f, 1-x*x-y*y)));
		pdf = wi.z/PI;
		if (wo.z<0) wi.z *= -1;
		return f(wo, wi);
	}
};

class BTDF : public BxDF {
public:
	virtual Color sample_f(const vec3& wo, vec3& wi, float& pdf) const {
		// sample upper hemisphere with p = cos theta
		float x,y;
		do {
			x = randf()*2-1;
			y = randf()*2-1;
		}
		while (x*x+y*y>1);
		wi = vec3(x,y,std::sqrt(std::max(0.0f, 1-x*x-y*y)));
		pdf = wi.z/PI;
		if (wo.z>0) wi.z *= -1;
		return f(wo, wi);
	}
};


class LambertBRDF : public BRDF {
private:
	Color albedo;
public:
	LambertBRDF(Color r): albedo(r) {
	}
	Color f(const vec3& wo, const vec3& wi) const {
		return 1/PI * albedo;
	}
};

class MirrorBRDF : public BRDF {
private:
	Color albedo;
public:
	MirrorBRDF(Color r): albedo(r) {
	}
	Color f(const vec3& wo, const vec3& wi) const {
		return Color(0);
	}
	Color sample_f(const vec3& wo, vec3& wi, float& pdf) const {
		wi = vec3(-wo.x, -wo.y, wo.z);
		pdf = 1;
		return 1 / fabs(wo.z) * albedo;
	}
};

class InvisibleBTDF : public BTDF {
public:
	Color f(const vec3& wo, const vec3& wi) const {
		return Color(0);
	}
	Color sample_f(const vec3& wo, vec3& wi, float& pdf) const {
		wi = -wo;
		pdf = 1;
		return 1 / fabs(wo.z);
	}
};