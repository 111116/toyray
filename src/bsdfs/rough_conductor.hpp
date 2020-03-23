#pragma once

#include "bxdf.hpp"
#include "microfacet.hpp"
#include "util/jsonutil.hpp"


class RoughConductorBRDF : public BRDF
{
private:
	Color eta, k, albedo;
	float alpha;
	
public:
	RoughConductorBRDF(Color eta, Color k, float roughness, Color albedo):
		alpha(roughness), eta(eta), k(k), albedo(albedo) {}
		
	RoughConductorBRDF(const Json& conf)
	{
		std::string name = "Cu";
		if (conf.find("material") != conf.end())
			name = conf["material"];
		alpha = conf["roughness"];
		albedo = 1;
		if (conf.find("albedo") != conf.end())
			albedo = json2vec3f(conf["albedo"]);
		bool found = false;
		for (int i=0; i<ComplexIorCount; ++i)
			if (complexIorList[i].name == name) {
				found = true;
				eta = complexIorList[i].eta;
				k = complexIorList[i].k;
			}
		if (!found) throw "conductor material not found";
		if (conf.find("eta") != conf.end())
			eta = json2vec3f(conf["eta"]);
		if (conf.find("k") != conf.end())
			k = json2vec3f(conf["k"]);
	}

	Color f(const vec3f& wo, const vec3f& wi) const override
	{
		vec3f wm = normalized(wo+wi); // microfacet normal
		float cos = dot(wm,wo);
		Color t1 = (eta*eta + k*k) * (cos*cos);
		Color r1 = (t1 - 2*eta*cos + 1) / (t1 + 2*eta*cos + 1);
		Color t2 = eta*eta + k*k;
		Color r2 = (t2 - 2*eta*cos + Color(cos*cos)) / (t2 + 2*eta*cos + Color(cos*cos));
		Color Fres = 0.5 * (r1*r1 + r2*r2); // reflected energy
		return EDX::GGX_D(wm,alpha) * EDX::Smith_G(wo,wi,wm,alpha) / (4 * dot(wo,wm) * dot(wi,wm)) * Fres * albedo;
	}

	// Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	// {
	// 	// https://agraphicsguy.wordpress.com/2015/11/01/sampling-microfacet-brdf/
	// 	float e = sampler.get1f();
	// 	float theta = atan(alpha*sqrtf(fmax(0,e/(1-e))));
	// 	float phi = 2*PI * sampler.get1f();
	// 	// convert to cartesian coordinates
	// 	vec3f wm (cos(theta)*sin(phi), cos(theta)*cos(phi), sin(theta));
	// 	wi = 2*dot(wm,wo)*wm-wo;
		// calculate pdf
		
	// 	// wi = vec3f(-wo.x, -wo.y, wo.z);
	// 	// float cos = fabs(wo.z);
	// 	// pdf = 1;
	// 	// Color t1 = (eta*eta + k*k) * (cos*cos);
	// 	// Color r1 = (t1 - 2*eta*cos + 1) / (t1 + 2*eta*cos + 1);
	// 	// Color t2 = eta*eta + k*k;
	// 	// Color r2 = (t2 - 2*eta*cos + Color(cos*cos)) / (t2 + 2*eta*cos + Color(cos*cos));
	// 	// return 0.5 / cos * (r1 + r2) * albedo;
	// }
};


