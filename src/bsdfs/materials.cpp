
#include "lib/consolelog.hpp"
#include "bsdf.hpp"
#include "util/jsonutil.hpp"

#include "matte.hpp" // lambert reflective
#include "mirror.hpp" // specular reflective
#include "invisible.hpp" // specular (no bending) refractive
#include "phong.hpp" // phong reflective
#include "conductor.hpp"
#include "dielectric.hpp"


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
		if (conf["type"] == "conductor")
			bsdf->add_component(new ConductorBRDF(conf["material"], json2vec3f(conf["albedo"])));
		if (conf["type"] == "dielectric") {
			bsdf->add_component(new DielectricBRDF(conf["ior"], json2vec3f(conf["albedo"])));
			bsdf->add_component(new DielectricBTDF(conf["ior"], json2vec3f(conf["albedo"])));
		}
		if (bsdf->empty())
			throw std::runtime_error("Unrecognized BSDF type " + std::string(conf["type"]));
	}
	catch (const char* err)
	{
		console.warn(err);
		console.info("Replacing with gray matte. Continueing...");
		delete bsdf;
		bsdf = new BSDF();
		bsdf->add_component(new LambertBRDF(Color(0.7)));
	}
	catch (std::runtime_error err)
	{
		console.warn(err.what());
		console.info("Replacing with gray matte. Continueing...");
		delete bsdf;
		bsdf = new BSDF();
		bsdf->add_component(new LambertBRDF(Color(0.7)));
	}
	return bsdf;
}
