#pragma once

#include "geometry.hpp"

class Sphere: public Primitive
{
private:
	point origin;
	float radius;

public:
	Sphere(point p, float r): origin(p), radius(r) {}

	bool intersect(Ray ray, point* result)
	{
		float l = norm(origin - ray.origin);
		if (dot(ray.dir, origin - ray.origin) < 1e-4) return false;
		if (norm(cross(ray.dir, origin - ray.origin)) >= radius) return false;

		float lcos = dot(ray.dir, origin - ray.origin);
		float tmp = lcos*lcos + radius*radius - l*l;
#ifdef DEBUG
		assert(abs(norm(ray.dir)-1) < 3e-6);
#endif
		// if (tmp <= -3e-5)
		// {
		// 	fprintf(stderr, "%f %f %f %f %f %f\n", ray.origin.x, ray.origin.y, ray.origin.z, ray.dir.x,ray.dir.y,ray.dir.z);
		// 	fprintf(stderr, "l=%f\n", l);
		// 	fprintf(stderr, "lcos=%f\n", lcos);
		// 	fprintf(stderr, "l??=%f\n", lcos*lcos + radius*radius - l*l);
		// }
		assert(tmp > -6e-5);
		if (tmp < 0) tmp = 0;
		float a=lcos, b=sqrt(tmp);
		float resl = a<b? a+b: a-b;
		// fprintf(stderr, "l??=%f\n", lcos*lcos + radius*radius - l*l);
		// fprintf(stderr, "resl=%f\n", resl);
		vec3 tar = ray.origin + ray.dir * resl - origin;
		// fprintf(stderr, "%f %f %f %f\n", tar.x, tar.y, tar.z, norm(tar));
#ifdef DEBUG
		assert(abs(norm(ray.origin + ray.dir * resl - origin) - radius) < 1e-4);
#endif
		*result = ray.origin + ray.dir * resl;
		return true;
	}

	vec3 normalAtPoint(point p)
	{
#ifdef DEBUG
		assert(abs(norm(p - origin) - radius) < 1e-4);
#endif
		return normalize(p - origin);
	}

	point surface_uniform_sample()
	{
		throw "not yet implemented";
	}

	float surfaceArea()
	{
		throw "not yet implemented";
	}

	AABox boundingVolume()
	{
		Volume v;
		v.x1 = origin.x - radius;
		v.x2 = origin.x + radius;
		v.y1 = origin.y - radius;
		v.y2 = origin.y + radius;
		v.z1 = origin.z - radius;
		v.z2 = origin.z + radius;
		return v;
	}
};


