#pragma once

#include "bxdf.hpp"
#include "microfacet.hpp"
#include "util/jsonutil.hpp"
#include "complexiordata.hpp"


class RoughConductorBRDF : public BRDF
{
private:
	Color eta, k;
	float alpha;
	
public:
	RoughConductorBRDF(Color eta, Color k, float roughness):
		alpha(roughness), eta(eta), k(k) {}
		
	RoughConductorBRDF(const Json& conf)
	{
		std::string name = "Cu";
		if (conf.find("material") != conf.end())
			name = conf["material"];
		alpha = conf["roughness"];
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

	Color f(const Color& albedo, const vec3f& wo, const vec3f& wi) const override
	{
		vec3f wm = normalized(wo+wi); // microfacet normal
		float cos = dot(wm,wo);
		Color t1 = (eta*eta + k*k) * (cos*cos);
		Color r1 = (t1 - 2*eta*cos + 1) / (t1 + 2*eta*cos + 1);
		Color t2 = eta*eta + k*k;
		Color r2 = (t2 - 2*eta*cos + Color(cos*cos)) / (t2 + 2*eta*cos + Color(cos*cos));
		Color Fres = 0.5 * (r1 + r2); // reflected energy
		return EDX::GGX_D(wm,alpha) * EDX::Smith_G(wo,wi,wm,alpha) / (4 * wo.z * wi.z) * Fres * albedo;
	}

	Color sample_f(const Color& albedo, const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		vec2f u = sampler.get2f();
    	vec3f wm = schuttejoe::GgxVndf(wo, alpha, u.x, u.y);
    	wi = schuttejoe::Reflect(wm, wo);
    	// if (sampler.get1f() < 0.0001) console.log(wi,wm,wo);
    	pdf = EDX::GGX_D(wm,alpha) * fabs(wm.z) / 4 / fabs(dot(wo, wm));
    	if (wi.z<0 ^ wo.z<0) return 0;
    	return f(albedo, wo, wi);
	}
};


