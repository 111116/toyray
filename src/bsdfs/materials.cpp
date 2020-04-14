
#include <unordered_map>
#include <string>
#include "util/jsonutil.hpp"
#include "lib/consolelog.hpp"
#include "bsdf.hpp"
#include "singlebsdf.hpp"
#include "transparency.hpp"

#include "lambert.hpp" // lambert reflective
#include "mirror.hpp" // specular reflective
#include "invisible.hpp" // specular (no bending) refractive
#include "phong.hpp" // phong reflective
#include "conductor.hpp"
#include "dielectric.hpp"
#include "rough_conductor.hpp"
#include "rough_plastic.hpp"
#include "plastic.hpp"


BSDF* newMaterial(const Json& conf, const std::unordered_map<std::string, BSDF*>& bsdfref)
{
	if (conf.type() == Json::value_t::string) {
		if (bsdfref.find(conf) == bsdfref.end())
			throw "undefined bsdf referenced";
		return bsdfref.find(conf)->second;
	}
	BSDF* bsdf = NULL;
	try {
		if (conf["type"] == "null")
			return new SingleBSDF(conf, NULL);
		if (conf["type"] == "lambert")
			bsdf = new SingleBSDF(conf, new LambertBRDF());
		if (conf["type"] == "lambert_transmit")
			bsdf = new SingleBSDF(conf, new LambertBTDF());
		if (conf["type"] == "phong")
			bsdf = new SingleBSDF(conf, new Phong(conf));
		if (conf["type"] == "mirror")
			bsdf = new SingleBSDF(conf, new MirrorBRDF());
		if (conf["type"] == "invisible")
			bsdf = new SingleBSDF(conf, new InvisibleBTDF());
		if (conf["type"] == "conductor")
			bsdf = new SingleBSDF(conf, new ConductorBRDF(conf["material"]));
		if (conf["type"] == "dielectric")
			bsdf = new SingleBSDF(conf, new DielectricBRDF(conf["ior"]));
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
			bsdf = new SingleBSDF(conf, new DielectricBRDF(conf["ior"]));
		}
		if (conf["type"] == "oren_nayar") {
			console.warn("oren_nayar unsupported");
			bsdf = new SingleBSDF(conf, new LambertBRDF());
		}
		if (conf["type"] == "transparency") {
			bsdf = new Transparency(conf, newMaterial(conf["base"], bsdfref));
		}
		if (conf["type"] == "translucent") {
			if (json2vec3f(conf["reflect"]) != vec3f(0)) throw "translucent unsupported";
			if (json2vec3f(conf["transmit"]) != vec3f(1)) throw "translucent unsupported";
			bsdf = new SingleBSDF(conf, new LambertBTDF());
		}
		if (conf["type"] == "uber") {
			console.warn("uber unsupported");
			bsdf = new SingleBSDF(conf, new LambertBRDF());
		}
		if (bsdf == NULL)
			throw std::runtime_error("Unrecognized BSDF type " + std::string(conf["type"]));
	}
	catch (const char* err)
	{
		console.warn(err);
		console.info("Replacing with gray matte. Continueing...");
		Json tmp;
		tmp["albedo"] = 0.7;
		bsdf = new SingleBSDF(tmp, new LambertBRDF());
	}
	catch (std::runtime_error err)
	{
		console.warn(err.what());
		console.info("Replacing with gray matte. Continueing...");
		Json tmp;
		tmp["albedo"] = 0.7;
		bsdf = new SingleBSDF(tmp, new LambertBRDF());
	}
	return bsdf;
}
