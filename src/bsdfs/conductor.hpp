#pragma once

#include "bxdf.hpp"
#include "complexiordata.hpp"


class ConductorBRDF : public BRDF, public DiracBxDF
{
private:
	Color eta, k, albedo;
	
public:
	ConductorBRDF(Color eta, Color k, Color albedo): eta(eta), k(k), albedo(albedo)
	{
		flags = Type(DIRAC | REFLECT);
	}
	ConductorBRDF(std::string name, Color albedo): albedo(albedo)
	{
		bool found = false;
		for (int i=0; i<ComplexIorCount; ++i)
			if (complexIorList[i].name == name) {
				found = true;
				eta = complexIorList[i].eta;
				k = complexIorList[i].k;
			}
		if (!found) throw "conductor material not found";
	}

	Color sample_f(const vec3f& wo, vec3f& wi, float& pdf, Sampler& sampler) const override
	{
		wi = vec3f(-wo.x, -wo.y, wo.z);
		float cos = fabs(wo.z);
		pdf = 1;
		Color t1 = (eta*eta + k*k) * (cos*cos);
		Color r1 = (t1 - 2*eta*cos + 1) / (t1 + 2*eta*cos + 1);
		Color t2 = eta*eta + k*k;
		Color r2 = (t2 - 2*eta*cos + Color(cos*cos)) / (t2 + 2*eta*cos + Color(cos*cos));
		return 0.5 / cos * (r1 + r2) * albedo;
	}
};


