#pragma once

#include "lib/json.hpp"
#include "env.hpp"
#include "mesh.h"
#include "bsdf.hpp"
#include "light.hpp"

using Json = nlohmann::json;

struct Object {
	BsDF* bsdf = NULL;
	Mesh* mesh = NULL;
	Light* emission = NULL;
	Object(const Json& conf, BsDF* bsdf) {
		assert(conf["type"] == "mesh");
		this->mesh = new Mesh();
		std::cout << "Loading model: " << modelpath + std::string(conf["file"]) << std::endl;
		this->mesh->loadFromFile((modelpath + std::string(conf["file"])).c_str());
		this->bsdf = bsdf;
		if (conf.find("emission") != conf.end()) {
			this->emission = new DiffuseAreaLight(json2vec3(conf["emission"]));
		}
	}
};