#pragma once

#include "color.h"
#include "bsdf.hpp"
#include "scalarimage.hpp"

class BxDF;

// wrapper of low-level BxDFs
class Transparency : public BSDF
{
private:
	BSDF const* base = NULL;
	ScalarImage* alphaTexture = NULL;

public:
	Transparency(const Json& conf, BSDF* base);
	Color f(const vec3f& wo, const vec3f& wi, const HitInfo& hit) const final;
	Color sample_f(const vec3f& wo, vec3f& wi, const HitInfo& hit, bool& isDirac, Sampler&) const final;
	bool isTransparent(const vec2f& uv) const;
};
