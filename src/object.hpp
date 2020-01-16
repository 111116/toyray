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
	bool samplable = true; // flag for light sampling; only valid when emission!=NULL
	Object(const Json& conf, BsDF* bsdf) {
		assert(conf["type"] == "mesh");
		this->mesh = new Mesh();
#pragma omp critical
		std::cout << "Loading model: " << modelpath + std::string(conf["file"]) << std::endl;
		this->mesh->loadFromFile((modelpath + std::string(conf["file"])).c_str());
		this->bsdf = bsdf;
		if (conf.find("emission") != conf.end()) {
			this->emission = new DiffuseAreaLight(json2vec3(conf["emission"]));
			if (conf.find("sample") != conf.end()) {
				samplable = conf["sample"];
			}
		}
	}
	point sample_point(float& pdf, Primitive*& shape) {
		shape = mesh->faces[rand() % mesh->faces.size()];
		pdf = 1.0 / mesh->faces.size() / shape->surfaceArea();
		return shape->surface_uniform_sample();
	}
};