#pragma once

#include "jsonutil.hpp"
#include "env.hpp"
#include "geometry/trianglemesh.hpp"
#include "geometry/sphere.hpp"
#include "geometry/plane.hpp"
#include "geometry/transform.hpp"
#include "bsdfs/bsdf.hpp"
#include "lights/light.hpp"
#include "transformparser.hpp"



struct Object
{
	BSDF* bsdf = NULL;
	Light* emission = NULL;
	bool samplable = true; // flag for light sampling; only valid when emission!=NULL
	// an object can contain either a primitive or a container of primitives
	Primitive* primitive = NULL;

	Object(const Json& conf, BSDF* bsdf)
	{
		// transform parsed here
		// pretransform if triangle / mesh TODO
		if (conf["type"] == "mesh") {
			primitive = new TriangleMesh(conf);
		}
		if (conf["type"] == "sphere") {
			primitive = new Sphere(json2vec3f(conf["origin"]), (double)conf["radius"]);
		}
		if (conf["type"] == "plane") {
			primitive = new Plane(json2vec3f(conf["normal"]), (double)conf["offset"]);
		}
		if (!primitive)
			throw "unrecognized geometric primitive type";
		if (conf.find("transform") != conf.end()) {
			mat4f m = parseTransform(conf["transform"]);
			if (m != mat4f::unit) {
				Primitive* t = primitive;
				primitive = new Transformed(t, m);
			}
		}
		this->bsdf = bsdf;
		// emission
		if (conf.find("emission") != conf.end()) {
			this->emission = new DiffuseAreaLight(json2vec3f(conf["emission"]));
			if (conf.find("sample") != conf.end()) {
				samplable = conf["sample"];
			}
		}
	}
	// point sample_point(float& pdf, Primitive*& shape) const {
	// 	if (container) {
	// 		shape = container->faces[rand() % container->faces.size()];
	// 		pdf = 1.0 / container->faces.size() / shape->surfaceArea();
	// 	}
	// 	else {
	// 		shape = primitive;
	// 		pdf = 1.0 / shape->surfaceArea();
	// 	}
	// 	return shape->surface_uniform_sample();
	// }
};
