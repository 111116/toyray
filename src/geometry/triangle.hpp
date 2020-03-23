#pragma once

#include "geometrybasic.hpp"
#include "math/matfloat.hpp"

class Triangle: public BasicPrimitive
{
	vec3f v1,v2,v3;
	vec3f vn1, vn2, vn3;
	vec2f vt1, vt2, vt3;

	mat3f tMatrix;
	vec3f planeNormal;
	float one_by_2S;

public:

	Triangle(vec3f v1, vec3f v2, vec3f v3, vec2f vt1, vec2f vt2, vec2f vt3, vec3f vn1, vec3f vn2, vec3f vn3):
		v1(v1), v2(v2), v3(v3), vn1(vn1), vn2(vn2), vn3(vn3), vt1(vt1), vt2(vt2), vt3(vt3)
	// pre-calculation to accelerate intersection / interpolation computation
	{
		// NOTE: be careful when dealing with tiny triangles!
		assert(norm(v1-v2)>0);
		assert(norm(v1-v3)>0);
		assert(norm(v2-v3)>0);
		assert(norm(cross(normalized(v2-v1), normalized(v3-v1))) > 1e-9);

		planeNormal = normalized(cross(v2-v1, v3-v1));
		// make sure plane normal points outward
		if (dot(planeNormal, vn1) < 0)
			planeNormal *= -1;
		one_by_2S = 1 / norm(cross(v3-v1, v2-v1));
		tMatrix = inverse(mat3f(v2-v1, v3-v1, cross(v2-v1, normalized(v3-v1))));
	}

	bool intersect(const Ray& ray, float& result) const
	{
		vec3f o = tMatrix * (ray.origin - v1);
		vec3f dir = tMatrix * ray.dir;
		float t = o.z / dir.z;
		vec3f p = o - dir * t;
		if (t<0 && p.x >= 0 && p.y >= 0 && p.x + p.y <= 1)
		{
			result = -t;
			return true;
		}
		return false;
	}

	vec3f Ns(const point& p) const
	{
		assert(onsurface(p));
		return normalized(
			vn1 + (vn3-vn1) * norm(cross(p-v1, v2-v1)) * one_by_2S
				+ (vn2-vn1) * norm(cross(p-v1, v3-v1)) * one_by_2S); // TO OPTIMIZE
	}

	vec2f uv(const point& p) const
	{
		assert(onsurface(p));
		return normalized(
			vt1 + (vt3-vt1) * norm(cross(p-v1, v2-v1)) * one_by_2S
				+ (vt2-vt1) * norm(cross(p-v1, v3-v1)) * one_by_2S); // TO OPTIMIZE
	}

	vec3f Ng(const point& p) const
	{
		return planeNormal;
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		vec2f t = sampler.sampleUnitTriangle();
		vec3f p = v1 + t.x * (v2-v1) + t.y * (v3-v1);
		return SampleInfo(p, planeNormal, 2 * one_by_2S);
	}

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
private:
	bool onsurface(const point& p) const {
		return norm(cross(p-v1,v2-v1)) + norm(cross(p-v2,v3-v2)) + norm(cross(p-v3,v1-v3)) < (1+1e-4) * norm(cross(v2-v1,v3-v1));
	}
};