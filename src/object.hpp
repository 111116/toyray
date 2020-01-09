#pragma once

#include "lib/json.hpp"
#include "env.hpp"
#include "mesh.h"
#include "bsdf.hpp"

using Json = nlohmann::json;

struct Object {
	BsDF* bsdf;
	Mesh* mesh;
	Object(const Json& conf, BsDF* bsdf) {
		assert(conf["type"] == "mesh");
		this->mesh = new Mesh();
		std::cout << "Loading model: " << modelpath + std::string(conf["file"]) << std::endl;
		this->mesh->loadFromFile((modelpath + std::string(conf["file"])).c_str());
		this->bsdf = bsdf;
	}
};