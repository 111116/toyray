#pragma once

#include "bsdf.hpp"
#include "phong.hpp"


BSDF* newMaterial(const Json& conf)
{
	BSDF* bsdf = new BSDF();
	if (conf["type"] == "lambert")
		bsdf->add_component(new LambertBRDF(json2vec3f(conf["albedo"])));
	if (conf["type"] == "phong")
		bsdf->add_component(new Phong(conf));
	// if (conf["type"] == "mirror")
	// 	bsdf->add_component(new MirrorBRDF(json2vec3f(conf["albedo"])));
	// if (conf["type"] == "invisible")
	// 	bsdf->add_component(new InvisibleBTDF());
	return bsdf;
}
