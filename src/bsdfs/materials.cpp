
#include "util/jsonutil.hpp"
#include "lib/consolelog.hpp"
#include "singlebsdf.hpp"

#include "matte.hpp" // lambert reflective
#include "mirror.hpp" // specular reflective
#include "invisible.hpp" // specular (no bending) refractive
#include "phong.hpp" // phong reflective
#include "conductor.hpp"
#include "dielectric.hpp"
#include "rough_conductor.hpp"
#include "rough_plastic.hpp"
#include "plastic.hpp"


BSDF* newMaterial(const Json& conf)
{
	BSDF* bsdf = NULL;
	try {
		if (conf["type"] == "null")
			return new SingleBSDF(conf, NULL);
		if (conf["type"] == "lambert")
			bsdf = new SingleBSDF(conf, new LambertBRDF(json2vec3f(conf["albedo"])));
		if (conf["type"] == "phong")
			bsdf = new SingleBSDF(conf, new Phong(conf));
		if (conf["type"] == "mirror")
			bsdf = new SingleBSDF(conf, new MirrorBRDF(json2vec3f(conf["albedo"])));
		if (conf["type"] == "invisible")
			bsdf = new SingleBSDF(conf, new InvisibleBTDF());
		if (conf["type"] == "conductor")
			bsdf = new SingleBSDF(conf, new ConductorBRDF(conf["material"], json2vec3f(conf["albedo"])));
		if (conf["type"] == "dielectric")
			bsdf = new SingleBSDF(conf, new DielectricBRDF(conf["ior"], json2vec3f(conf["albedo"])));
		if (conf["type"] == "rough_conductor") {
			console.warn("rough_conductor using naive sampling");
			bsdf = new SingleBSDF(conf, new RoughConductorBRDF(conf));
		}
		if (conf["type"] == "plastic")
			bsdf = new PlasticBSDF(conf);
		if (conf["type"] == "rough_plastic") {
			console.warn("plastic roughness is faked");
			bsdf = new RoughPlasticBSDF(conf);
		}
		if (conf["type"] == "rough_dielectric") {
			console.warn("dielectric roughness unsupported");
			bsdf = new SingleBSDF(conf, new DielectricBRDF(conf["ior"], json2vec3f(conf["albedo"])));
		}
		if (bsdf == NULL)
			throw std::runtime_error("Unrecognized BSDF type " + std::string(conf["type"]));
	}
	catch (const char* err)
	{
		console.warn(err);
		console.info("Replacing with gray matte. Continueing...");
		bsdf = new SingleBSDF(conf, new LambertBRDF(Color(0.7)));
	}
	catch (std::runtime_error err)
	{
		console.warn(err.what());
		console.info("Replacing with gray matte. Continueing...");
		bsdf = new SingleBSDF(conf, new LambertBRDF(Color(0.7)));
	}
	return bsdf;
}
