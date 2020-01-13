#pragma once

#define DEBUG

#ifdef DEBUG
#include <assert.h>
#endif

#include "la.h"

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
	virtual point sampleOnSurface() const = 0;
	virtual float surfaceArea() const = 0;
	virtual AABox boundingVolume() const = 0;
};


// class Sphere: public Primitive
// {
// private:
// 	point origin;
// 	float radius;

// public:
// 	Sphere(point p, float r): origin(p), radius(r) {}

// 	bool intersect(Ray ray, point* result)
// 	{
// 		float l = norm(origin - ray.origin);
// 		if (dot(ray.dir, origin - ray.origin) < 1e-4) return false;
// 		if (norm(cross(ray.dir, origin - ray.origin)) >= radius) return false;

// 		float lcos = dot(ray.dir, origin - ray.origin);
// 		float tmp = lcos*lcos + radius*radius - l*l;
// #ifdef DEBUG
// 		assert(abs(norm(ray.dir)-1) < 3e-6);
// #endif
// 		// if (tmp <= -3e-5)
// 		// {
// 		// 	fprintf(stderr, "%f %f %f %f %f %f\n", ray.origin.x, ray.origin.y, ray.origin.z, ray.dir.x,ray.dir.y,ray.dir.z);
// 		// 	fprintf(stderr, "l=%f\n", l);
// 		// 	fprintf(stderr, "lcos=%f\n", lcos);
// 		// 	fprintf(stderr, "l??=%f\n", lcos*lcos + radius*radius - l*l);
// 		// }
// 		assert(tmp > -6e-5);
// 		if (tmp < 0) tmp = 0;
// 		float a=lcos, b=sqrt(tmp);
// 		float resl = a<b? a+b: a-b;
// 		// fprintf(stderr, "l??=%f\n", lcos*lcos + radius*radius - l*l);
// 		// fprintf(stderr, "resl=%f\n", resl);
// 		vec3 tar = ray.origin + ray.dir * resl - origin;
// 		// fprintf(stderr, "%f %f %f %f\n", tar.x, tar.y, tar.z, norm(tar));
// #ifdef DEBUG
// 		assert(abs(norm(ray.origin + ray.dir * resl - origin) - radius) < 1e-4);
// #endif
// 		*result = ray.origin + ray.dir * resl;
// 		return true;
// 	}

// 	vec3 normalAtPoint(point p)
// 	{
// #ifdef DEBUG
// 		assert(abs(norm(p - origin) - radius) < 1e-4);
// #endif
// 		return normalize(p - origin);
// 	}

// 	point sampleOnSurface()
// 	{
// 		throw "not yet implemented";
// 	}

// 	float surfaceArea()
// 	{
// 		throw "not yet implemented";
// 	}

// 	// AABox boundingVolume()
// 	// {
// 	// 	Volume v;
// 	// 	v.x1 = origin.x - radius;
// 	// 	v.x2 = origin.x + radius;
// 	// 	v.y1 = origin.y - radius;
// 	// 	v.y2 = origin.y + radius;
// 	// 	v.z1 = origin.z - radius;
// 	// 	v.z2 = origin.z + radius;
// 	// 	return v;
// 	// }
// };






class Triangle: public Primitive
{
public:
	vec3 v1,v2,v3;
	vec3 vn1, vn2, vn3;
	vec2 vt1, vt2, vt3;

	vec3 planeNormal;
	mat3 interpMatrix, tMatrix;
	// AABox bv;

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
		interpMatrix = mat3(vn1,vn2,vn3) * inverse(mat3(v1,v2,v3));
		tMatrix = inverse(mat3(v2-v1, v3-v1, cross(v2-v1, normalize(v3-v1))));
		// bv = boundingVolume();
	}

	bool intersect(Ray r, point* result) const
	{
		// if (!bv.intersect(r)) return false;
		vec3 o = tMatrix * (r.origin - v1);
		vec3 dir = tMatrix * r.dir;
		if (o.z>0 ^ dir.z<0) return false;
		float t = o.z / dir.z;
		vec3 p = o - dir * t;
		if (p.x >= 0 && p.y >= 0 && p.x + p.y <= 1)
		{
			*result = v1 + p.x * (v2-v1) + p.y * (v3-v1);
			return true;
		}
		return false;
	}

	vec3 Ns(point p) const
	{
		return planeNormal; // TODO
		// interpolated normal:
		// return interpMatrix * p;
	}

	vec3 Ng(point p) const
	{
		return planeNormal;
	}

	point sampleOnSurface() const
	{
		float a=randf(), b=randf();
		if (a+b>1) a=1-a, b=1-b;
		return v1 + a*(v2-v1) + b*(v3-v1);
	}
	float surfaceArea() const
	{
		return norm(cross(v2-v1, v3-v1))/2;
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
};


