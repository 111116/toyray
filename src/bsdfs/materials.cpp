
#include "lib/consolelog.hpp"
#include "singlebsdf.hpp"
#include "glassbsdf.hpp"
#include "util/jsonutil.hpp"

#include "matte.hpp" // lambert reflective
#include "mirror.hpp" // specular reflective
#include "invisible.hpp" // specular (no bending) refractive
#include "phong.hpp" // phong reflective
#include "conductor.hpp"
#include "dielectric.hpp"


BSDF* newMaterial(const Json& conf)
{
	BSDF* bsdf = NULL;
	try {
		if (conf["type"] == "null")
			return new SingleBSDF(NULL);
		if (conf["type"] == "lambert")
			bsdf = new SingleBSDF(new LambertBRDF(json2vec3f(conf["albedo"])));
		if (conf["type"] == "phong")
			bsdf = new SingleBSDF(new Phong(conf));
		if (conf["type"] == "mirror")
			bsdf = new SingleBSDF(new MirrorBRDF(json2vec3f(conf["albedo"])));
		if (conf["type"] == "invisible")
			bsdf = new SingleBSDF(new InvisibleBTDF());
		if (conf["type"] == "conductor")
			bsdf = new SingleBSDF(new ConductorBRDF(conf["material"], json2vec3f(conf["albedo"])));
		if (conf["type"] == "dielectric")
			bsdf = new GlassBSDF(conf["ior"]);
		if (bsdf == NULL)
			throw std::runtime_error("Unrecognized BSDF type " + std::string(conf["type"]));
	}
	catch (const char* err)
	{
		console.warn(err);
		console.info("Replacing with gray matte. Continueing...");
		bsdf = new SingleBSDF(new LambertBRDF(Color(0.7)));
	}
	catch (std::runtime_error err)
	{
		console.warn(err.what());
		console.info("Replacing with gray matte. Continueing...");
		bsdf = new SingleBSDF(new LambertBRDF(Color(0.7)));
	}
	return bsdf;
}
