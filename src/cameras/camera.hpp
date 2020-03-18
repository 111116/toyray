#pragma once

#include "util/jsonutil.hpp"
#include "../geometry/geometry.hpp"

struct Camera
{
	int resx; // number of pixels in a horizontal line
	int resy; // number of pixels in a vertical line
	vec3f position; // position of observer
	vec3f look_at; // the point being looked at
	vec3f up; // upward vector
	float fov; // in radians

	Camera(const Json& conf) {
		std::vector<int> resolution = conf["resolution"];
		resx = resolution[0];
		resy = resolution[1];
		position = json2vec3f(conf["transform"]["position"]);
		look_at = json2vec3f(conf["transform"]["look_at"]);
		up = normalized(json2vec3f(conf["transform"]["up"]));
		float fov_degree = conf["fov"];
		fov = fov_degree / 180 * PI;
	}
	// u: 0..1 left-right
	// v: 0..1 up-down
	virtual Ray sampleray(vec2f pos) const = 0;
};

struct PinholeCamera : public Camera
{
	vec3f ray_central, uphalf, lefthalf;

	PinholeCamera(const Json& conf): Camera(conf) {
		ray_central = look_at - position;
		uphalf = norm(ray_central) * resy / resx * tan(fov/2) * up;
		lefthalf = cross(up, ray_central) * tan(fov/2);
	}
	
	Ray sampleray(vec2f pos) const {
		return Ray(position, normalized(ray_central + (1-pos.x*2) * lefthalf + (1-pos.y*2) * uphalf));
	}
};

Camera* newCamera(const Json& conf) {
	if (conf["type"] == "pinhole")
		return new PinholeCamera(conf);
	throw "unrecognized camera type";
}