#pragma once

#include "jsonutil.hpp"
#include "geometry/trianglemesh.hpp"
#include "geometry/sphere.hpp"
#include "geometry/plane.hpp"
#include "geometry/transform.hpp"
#include "bsdfs/bsdf.hpp"
#include "lights/lighttypes.hpp"
#include "transformparser.hpp"



struct Object
{
	BSDF* bsdf = NULL;
	Light* emission = NULL;
	// an object can contain either a primitive or a container of primitives
	Primitive* primitive = NULL;

	Object(const Json& conf, BSDF* bsdf, Primitive* instancing = NULL)
	{
		this->bsdf = bsdf;
		// primitive construct (use instancing if applicable)
		if (instancing != NULL) {
			primitive = instancing;
		}
		else {
			if (conf["type"] == "mesh") {
				primitive = new TriangleMesh(conf);
			}
			if (conf["type"] == "sphere") {
				primitive = new Sphere(json2vec3f(conf["origin"]), (double)conf["radius"]);
			}
			if (conf["type"] == "plane") {
				primitive = new Plane(json2vec3f(conf["normal"]), (double)conf["offset"]);
			}
		}
		if (!primitive) {
			throw "unrecognized geometric primitive type";
		}
		// apply transform
		if (conf.find("transform") != conf.end()) {
			mat4f m = parseTransform(conf["transform"]);
			if (m != mat4f::unit) {
				Primitive* t = primitive;
				primitive = new Transformed(t, m);
			}
		}
		// apply emission
		if (conf.find("emission") != conf.end()) {
			bool samplable = false;
			if (conf.find("sample") != conf.end()) {
				samplable = conf["sample"];
			}
			// TODO
			samplable = false; // override
			this->emission = new DiffuseLight(samplable, json2vec3f(conf["emission"]), primitive);
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
