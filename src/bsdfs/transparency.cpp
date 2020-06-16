
#include "bxdf.hpp"
#include "transparency.hpp"
#include "materials.hpp"

Transparency::Transparency(const Json& conf, BSDF* base):
	BSDF(conf["base"]), base(base)
{
	alphaTexture = new ScalarImage(getpath(conf["alpha"]));
	if (base == NULL)
		throw "transparency: unrecognized base material";
}

Color Transparency::f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const
{
	// float alpha = alphaTexture->sample(vec2f(hit.uv.x,-hit.uv.y));
	return base->f(wo, wi, hit);
}

Color Transparency::sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler& sampler) const
{
	return base->sample_f(wo, wi, hit, isDirac, sampler);
}

bool Transparency::isTransparent(const vec2f& uv) const
{
	return alphaTexture->sample(vec2f(uv.x, -uv.y)) < 0.01;
}
