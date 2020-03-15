#pragma once

#include <iostream>
#include "bsdf.hpp"

#include "matte.hpp" // lambert reflective
#include "mirror.hpp" // specular reflective
#include "invisible.hpp" // specular (no bending) refractive
#include "phong.hpp" // phong reflective


BSDF* newMaterial(const Json& conf)
{
	BSDF* bsdf = new BSDF();
	if (conf["type"] == "null")
		return bsdf;
	if (conf["type"] == "lambert")
		bsdf->add_component(new LambertBRDF(json2vec3f(conf["albedo"])));
	if (conf["type"] == "phong")
		bsdf->add_component(new Phong(conf));
	if (conf["type"] == "mirror")
		bsdf->add_component(new MirrorBRDF(json2vec3f(conf["albedo"])));
	if (conf["type"] == "invisible")
		bsdf->add_component(new InvisibleBTDF());
	if (bsdf->empty())
		std::cerr << "WARNING: UNRECOGNIZED BSDF TYPE " + std::string(conf["type"]) << std::endl;
	return bsdf;
}
