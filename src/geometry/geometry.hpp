#pragma once

#define DEBUG

#ifdef DEBUG
#include <assert.h>
#endif

#include "../la.h"

typedef vec3 point;

struct Ray
{
	point origin;
	vec3 dir; // must be normalized
	Ray(point p, vec3 d): origin(p), dir(d) {}
};


struct AABox
{
	float x1,x2,y1,y2,z1,z2;
	bool intersect(Ray r)
	{
		float tmin, tmax;
		if (r.dir.x > 0)
			tmin = (x1 - r.origin.x) / r.dir.x,
			tmax = (x2 - r.origin.x) / r.dir.x;
		else
			tmin = (x2 - r.origin.x) / r.dir.x,
			tmax = (x1 - r.origin.x) / r.dir.x;
		if (r.dir.y > 0)
			tmin = std::max(tmin, (y1 - r.origin.y) / r.dir.y),
			tmax = std::min(tmax, (y2 - r.origin.y) / r.dir.y);
		else
			tmin = std::max(tmin, (y2 - r.origin.y) / r.dir.y),
			tmax = std::min(tmax, (y1 - r.origin.y) / r.dir.y);
		if (r.dir.z > 0)
			tmin = std::max(tmin, (z1 - r.origin.z) / r.dir.z),
			tmax = std::min(tmax, (z2 - r.origin.z) / r.dir.z);
		else
			tmin = std::max(tmin, (z2 - r.origin.z) / r.dir.z),
			tmax = std::min(tmax, (z1 - r.origin.z) / r.dir.z);
		return tmin <= tmax;
	}
	float surfaceArea() {
		float dx = x2 - x1;
		float dy = y2 - y1;
		float dz = z2 - z1;
		return 2 * (dx*dy + dx*dz + dy*dz);
	}
};

AABox operator+ (const AABox& a, const AABox& b) {
	AABox t;
	t.x1 = std::min(a.x1, b.x1);
	t.x2 = std::max(a.x2, b.x2);
	t.y1 = std::min(a.y1, b.y1);
	t.y2 = std::max(a.y2, b.y2);
	t.z1 = std::min(a.z1, b.z1);
	t.z2 = std::max(a.z2, b.z2);
	return t;
}


class Primitive
{
public:
	virtual bool intersect(Ray ray, point* result) const = 0;
	virtual vec3 Ns(point p) const = 0; // shader normal (interpolated from vertex normals)
	virtual vec3 Ng(point p) const = 0; // geometry normal (calculated from vertex positions)
	virtual point surface_uniform_sample() const = 0;
	virtual float surfaceArea() const = 0;
	virtual AABox boundingVolume() const = 0;
};