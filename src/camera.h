#pragma once

#include "jsonutil.hpp"
#include "geometry.h"

struct Camera
{
	int resolution_x; // number of pixels in a horizontal line
	int resolution_y; // number of pixels in a vertical line
	vec3 position; // position of observer
	vec3 look_at; // the point being looked at
	vec3 up; // upward vector
	float fov; // in radians

	Camera(const Json& conf) {
		std::vector<int> resolution = conf["resolution"];
		resolution_x = resolution[0];
		resolution_y = resolution[1];
		position = json2vec3(conf["transform"]["position"]);
		look_at = json2vec3(conf["transform"]["look_at"]);
		up = normalize(json2vec3(conf["transform"]["up"]));
		float fov_degree = conf["fov"];
		fov = fov_degree / 180 * PI;
	}
	// u: 0..1 left-right
	// v: 0..1 up-down
	virtual Ray sampleray(float u, float v) = 0;
};

struct PinholeCamera : public Camera
{
	vec3 ray_central, uphalf, lefthalf;

	PinholeCamera(const Json& conf): Camera(conf) {
		assert(conf["type"] == "pinhole");
		ray_central = look_at - position;
		uphalf = norm(ray_central) * resolution_y / resolution_x * tan(fov/2) * up;
		lefthalf = cross(up, ray_central) * tan(fov/2);
	}
	
	Ray sampleray(float u, float v) {
		return Ray(position, normalize(ray_central + (1-u*2) * lefthalf + (1-v*2) * uphalf));
	}
};