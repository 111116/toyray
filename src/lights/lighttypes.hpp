#pragma once

#include "pointlight.hpp"
#include "constlight.hpp"
#include "directionallight.hpp"
#include "../jsonutil.hpp"

Light* newLight(const Json& conf) {
	if (conf["type"] == "point")
		return new PointLight(json2vec3f(conf["intensity"]), json2vec3f(conf["position"]));
	if (conf["type"] == "directional")
		return new DirectionalLight(json2vec3f(conf["irradiance"]), json2vec3f(conf["direction"]));
	if (conf["type"] == "constant")
		return new ConstEnvLight(json2vec3f(conf["radiance"]));
	throw "unrecognized type of light source";
}