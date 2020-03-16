#pragma once

#include "trianglemesh.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "quad.hpp"
#include "../lib/json.hpp"
#include "../transformparser.hpp"

Primitive* newPrimitive(const Json& conf)
{
	Primitive* shape = NULL;
	if (conf["type"] == "mesh") {
		shape = new TriangleMesh(conf);
	}
	if (conf["type"] == "sphere") {
		shape = new Sphere(json2vec3f(conf["origin"]), (double)conf["radius"]);
	}
	if (conf["type"] == "plane") {
		shape = new Plane(json2vec3f(conf["normal"]), (double)conf["offset"]);
	}
	if (conf["type"] == "quad") {
		mat4f trans = mat4f::unit;
		if (conf.find("transform") != conf.end())
			trans = parseTransform(conf["transform"]);
		shape = new Quad(trans);
	}
	if (shape == NULL) {
		throw std::runtime_error("Unrecognized Geometric type: " + std::string(conf["type"]));
	}
	return shape;
}