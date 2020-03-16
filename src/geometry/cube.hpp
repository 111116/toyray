#pragma once

#include "geometry.hpp"
#include "../math/matfloat.hpp"

class Cube: public Primitive
{
	static constexpr float x1 = -0.5;
	static constexpr float x2 =  0.5;
	static constexpr float y1 = -0.5;
	static constexpr float y2 =  0.5;
	static constexpr float z1 = -0.5;
	static constexpr float z2 =  0.5;
public:

	Cube(){}

	bool intersect(const Ray& ray, Hit* result) const
	{
		float tmin, tmax;
		vec3f resN = vec3f(1,0,0);

		if (ray.dir.x > 0) {
			tmin = (x1 - ray.origin.x) / ray.dir.x;
			tmax = (x2 - ray.origin.x) / ray.dir.x;
		}
		else {
			tmin = (x2 - ray.origin.x) / ray.dir.x;
			tmax = (x1 - ray.origin.x) / ray.dir.x;
		}
		if (ray.dir.y > 0) {
			float tminy = (y1 - ray.origin.y) / ray.dir.y;
			if (tminy > tmin) {
				tmin = tminy;
				resN = vec3f(0,1,0);
			}
			tmax = std::min(tmax, (y2 - ray.origin.y) / ray.dir.y);
		}
		else {
			float tminy = (y2 - ray.origin.y) / ray.dir.y;
			if (tminy > tmin) {
				tmin = tminy;
				resN = vec3f(0,1,0);
			}
			tmax = std::min(tmax, (y1 - ray.origin.y) / ray.dir.y);
		}
		if (ray.dir.z > 0) {
			float tminz = (z1 - ray.origin.z) / ray.dir.z;
			if (tminz > tmin) {
				tmin = tminz;
				resN = vec3f(0,0,1);
			}
			tmax = std::min(tmax, (z2 - ray.origin.z) / ray.dir.z);
		}
		else {
			float tminz = (z2 - ray.origin.z) / ray.dir.z;
			if (tminz > tmin) {
				tmin = tminz;
				resN = vec3f(0,0,1);
			}
			tmax = std::min(tmax, (z1 - ray.origin.z) / ray.dir.z);
		}
		if (tmin <= tmax)
		{
			*result = Hit(ray.atParam(tmin), resN, resN);
			return true;
		}
		return false;
	}

	AABox boundingVolume() const
	{
		return AABox(-0.5,0.5,-0.5,0.5,-0.5,0.5);
	}
};