#pragma once

#include "geometry.hpp"

class Sphere: public Primitive
{
private:
	point center;
	float radius;

public:
	Sphere(point p, float r): center(p), radius(r) {}

	bool intersect(const Ray& ray, point* result) const
	{
        vec3f OC = center - ray.origin;
        float midpointT = dot(OC, ray.dir);
        vec3f midpoint = ray.atParam(midpointT);
        float sqrRmid = sqrlen(midpoint - center);
        if (sqrRmid >= radius * radius) return false;
        float tanH = std::sqrt(radius * radius - sqrRmid);
        float lambda = midpointT - tanH;
        if (lambda >= 0) { // intersecting outer surface
            *result = ray.atParam(lambda);
            return true;
        }
        lambda = midpointT + tanH;
        if (lambda >= 0) { // intersection inner surface
            *result = ray.atParam(lambda);
            return true;
        }
        return false;
	}

	vec3 Ns(const point& p) const
	{
#ifdef DEBUG
		assert(abs(norm(p - center) - radius) < 1e-2);
#endif
		return normalize(p - center);
	}

	vec3 Ng(const point& p) const
	{
#ifdef DEBUG
		assert(abs(norm(p - center) - radius) < 1e-2);
#endif
		return normalize(p - center);
	}

	// point surface_uniform_sample() const
	// {
	// 	return center + radius * randunitvec3f();
	// }

	// float surfaceArea() const
	// {
	// 	return 4*PI * radius * radius;
	// }

	AABox boundingVolume() const
	{
		AABox v;
		v.x1 = center.x - radius;
		v.x2 = center.x + radius;
		v.y1 = center.y - radius;
		v.y2 = center.y + radius;
		v.z1 = center.z - radius;
		v.z2 = center.z + radius;
		return v;
	}
};


