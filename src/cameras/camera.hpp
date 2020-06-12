#pragma once

#include "../geometry/geometry.hpp"

struct Camera
{
	int resx; // number of pixels in a horizontal line
	int resy; // number of pixels in a vertical line
	vec3f position; // position of observer
	vec3f look_at; // the point being looked at
	vec3f up; // upward vector
	float fov; // in radians
	bool mirror = false; // horizontally mirrored

	Camera(const Json& conf) {
		std::vector<int> resolution = conf["resolution"];
		resx = resolution[0];
		resy = resolution[1];
		position = json2vec3f(conf["transform"]["position"]);
		look_at = json2vec3f(conf["transform"]["look_at"]);
		up = normalized(json2vec3f(conf["transform"]["up"]));
		if (conf["transform"].find("mirror") != conf["transform"].end())
			mirror = conf["transform"]["mirror"];
		float fov_degree = conf["fov"];
		fov = fov_degree / 180 * PI;
	}
	// u: 0..1 left-right
	// v: 0..1 up-down
	virtual Ray sampleray(vec2f pos, Sampler&) const = 0;
};
