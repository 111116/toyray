#pragma once

#include "lib/json.hpp"
#include "env.hpp"
#include "mesh.h"
#include "bsdf.hpp"
#include "light.hpp"

using Json = nlohmann::json;

struct Object {
	BSDF* bsdf = NULL;
	Mesh* mesh = NULL;
	Light* emission = NULL;
	bool samplable = true; // flag for light sampling; only valid when emission!=NULL
	Object(const Json& conf, BSDF* bsdf) {
		assert(conf["type"] == "mesh");
		this->mesh = new Mesh();
#pragma omp critical
		std::cout << "Loading model: " << modelpath + std::string(conf["file"]) << std::endl;
		this->mesh->loadFromFile((modelpath + std::string(conf["file"])).c_str());
		this->bsdf = bsdf;
		// emission
		if (conf.find("emission") != conf.end()) {
			this->emission = new DiffuseAreaLight(json2vec3(conf["emission"]));
			if (conf.find("sample") != conf.end()) {
				samplable = conf["sample"];
			}
		}
		// recompute normal
		bool recompute_normals = true;
		if (conf.find("recompute_normals") != conf.end()) {
			recompute_normals = conf["recompute_normals"];
		}
		if (recompute_normals) {
			mesh->recompute_normals();
		}
	}
	point sample_point(float& pdf, Primitive*& shape) const {
		shape = mesh->faces[rand() % mesh->faces.size()];
		pdf = 1.0 / mesh->faces.size() / shape->surfaceArea();
		return shape->surface_uniform_sample();
	}
};