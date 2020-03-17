#pragma once

#include "geometry.hpp"

class Sphere: public BasicPrimitive
{
private:
	point center;
	float radius;

public:
	Sphere(point p, float r): center(p), radius(r) {}
	bool intersect(const Ray& ray, float& result) const
	{
        vec3f OC = center - ray.origin;
        float midpointT = dot(OC, ray.dir);
        vec3f midpoint = ray.atParam(midpointT);
        float sqrRmid = sqrlen(midpoint - center);
        if (sqrRmid >= radius * radius) return false;
        float tanH = std::sqrt(radius * radius - sqrRmid);
        float lambda = midpointT - tanH;
        if (lambda >= 0) { // intersecting outer surface
            result = lambda;
            return true;
        }
        lambda = midpointT + tanH;
        if (lambda >= 0) { // intersection inner surface
            result = lambda;
            return true;
        }
        return false;
	}

	vec3f Ns(const point& p) const
	{
		assert(fabs(norm(p - center) - radius) < 1e-2);
		return normalized(p - center);
	}

	vec3f Ng(const point& p) const
	{
		assert(fabs(norm(p - center) - radius) < 1e-2);
		return normalized(p - center);
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		vec3f N = sampler.sampleUnitSphereSurface();
		return SampleInfo(center + radius * N, N, 1.0 / (4*PI * radius * radius));
	}

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


