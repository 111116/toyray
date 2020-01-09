#pragma once

#include "jsonutil.hpp"

class BsDF {
public:
	vec3 albedo;
	BsDF(const Json& conf) {
		assert(conf["type"] == "lambert" || conf["type"] == "null");
		if (is_number(conf["albedo"])) {
			float t = conf["albedo"];
			albedo = vec3(t,t,t);
		}
		else {
			albedo = json2vec3(conf["albedo"]);
		}
	}
};