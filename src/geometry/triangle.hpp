#pragma once

#include "geometry.hpp"

class Triangle: public Primitive
{
	vec3 v1,v2,v3;
	vec3 vn1, vn2, vn3;
	vec2 vt1, vt2, vt3;

	mat3 tMatrix;
	vec3 planeNormal;
	float one_by_2S;
public:

	Triangle(vec3 v1, vec3 v2, vec3 v3, vec2 vt1, vec2 vt2, vec2 vt3, vec3 vn1, vec3 vn2, vec3 vn3):
		v1(v1), v2(v2), v3(v3), vn1(vn1), vn2(vn2), vn3(vn3), vt1(vt1), vt2(vt2), vt3(vt3)
	// pre-calculation to accelerate intersection / interpolation computation
	{
#ifdef DEBUG
		// NOTE: be careful when dealing with tiny triangles!
		assert(norm(v1-v2)>0);
		assert(norm(v1-v3)>0);
		assert(norm(v2-v3)>0);
		assert(norm(cross(normalize(v2-v1), normalize(v3-v1))) > 1e-5);
#endif
		planeNormal = normalize(cross(v2-v1, v3-v1));
		// make sure plane normal points outward
		if (dot(planeNormal, vn1) < 0)
			planeNormal *= -1;
		one_by_2S = 1 / norm(cross(v3-v1, v2-v1));
		tMatrix = inverse(mat3(v2-v1, v3-v1, cross(v2-v1, normalize(v3-v1))));
	}

	bool intersect(const Ray& ray, point* result) const
	{
		vec3 o = tMatrix * (ray.origin - v1);
		vec3 dir = tMatrix * ray.dir;
		float t = o.z / dir.z;
		vec3 p = o - dir * t;
		if (t>0 && p.x >= 0 && p.y >= 0 && p.x + p.y <= 1)
		{
			*result = v1 + p.x * (v2-v1) + p.y * (v3-v1);
			return true;
		}
		return false;
	}

	vec3 Ns(const point& p) const
	{
		return normalize(
			vn1 + (vn3-vn1) * norm(cross(p-v1, v2-v1)) * one_by_2S
				+ (vn2-vn1) * norm(cross(p-v1, v3-v1)) * one_by_2S); // TO OPTIMIZE
	}

	vec3 Ng(const point& p) const
	{
		return planeNormal;
	}

	// point surface_uniform_sample() const
	// {
	// 	float a=randf(), b=randf();
	// 	if (a+b>1) a=1-a, b=1-b;
	// 	return v1 + a*(v2-v1) + b*(v3-v1);
	// }
	// float surfaceArea() const
	// {
	// 	return norm(cross(v2-v1, v3-v1))/2;
	// }

	AABox boundingVolume() const
	{
		AABox v;
		v.x1 = std::min(v1.x, std::min(v2.x, v3.x));
		v.x2 = std::max(v1.x, std::max(v2.x, v3.x));
		v.y1 = std::min(v1.y, std::min(v2.y, v3.y));
		v.y2 = std::max(v1.y, std::max(v2.y, v3.y));
		v.z1 = std::min(v1.z, std::min(v2.z, v3.z));
		v.z2 = std::max(v1.z, std::max(v2.z, v3.z));
		return v;
	}

	void recompute_normal() {
		vn1 = vn2 = vn3 = planeNormal;
	}
};