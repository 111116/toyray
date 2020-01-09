#pragma once

#include "jsonutil.hpp"
#include "geometry.h"

struct Camera
{
	int resolution_x;
	int resolution_y;
	vec3 position;
	vec3 look_at;
	vec3 up;
	float fov;
	vec3 ray_central, uphalf, lefthalf;

	Camera(const Json& conf) {
		assert(conf["type"] == "pinhole");
		
		std::vector<int> resolution = conf["resolution"];
		resolution_x = resolution[0];
		resolution_y = resolution[1];
		position = json2vec3(conf["transform"]["position"]);
		look_at = json2vec3(conf["transform"]["look_at"]);
		up = normalize(json2vec3(conf["transform"]["up"]));
		float fov_degree = conf["fov"];
		fov = fov_degree / 180 * PI;
		ray_central = look_at - position;
		uphalf = norm(ray_central) * resolution_y / resolution_x * tan(fov/2) * up;
		lefthalf = cross(up, ray_central) * tan(fov/2);
		std::cout << ray_central << std::endl;
		std::cout << uphalf << std::endl;
		std::cout << lefthalf << std::endl;
	}
	// u: 0..1 left-right
	// v: 0..1 up-down
	Ray sampleray(float u, float v) {
		// return Ray(position, vec3(0,0,1)+(0.5-u)*vec3(1,0,0)+(0.5-v)*vec3(0,1,0));
		return Ray(position, vec3(ray_central + (1-u*2) * lefthalf + (1-v*2) * uphalf));
	}
};