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
	try {
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
			throw std::runtime_error("Unrecognized BSDF type " + std::string(conf["type"]));
	}
	catch (const char* err)
	{
		std::cerr << "ERROR: " << err << std::endl;
		std::cerr << "Replacing with gray matte. Continueing..." << std::endl;
		delete bsdf;
		bsdf = new BSDF();
		bsdf->add_component(new LambertBRDF(Color(0.7)));
	}
	catch (std::runtime_error err)
	{
		std::cerr << "ERROR: " << err.what() << std::endl;
		std::cerr << "Replacing with gray matte. Continueing..." << std::endl;
		delete bsdf;
		bsdf = new BSDF();
		bsdf->add_component(new LambertBRDF(Color(0.7)));
	}
	return bsdf;
}
