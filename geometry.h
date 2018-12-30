#ifndef GEOMETRY_H
#define GEOMETRY_H

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
	// Ray(point p, vec3 d): origin(p), dir(d) {}
};


struct Sphere
{
	point origin;
	float radius;
	Sphere(point p, float r): origin(p), radius(r) {}

	bool hasIntersection(Ray ray)
	{
		float l = norm(origin - ray.origin);
		// DEBUG!!!
		if (dot(ray.dir, origin - ray.origin) < 1e-4) return false;
		return norm(cross(ray.dir, origin - ray.origin)) < radius;
	}
	
	point intersection(Ray ray)
	{
		float l = norm(origin - ray.origin);
		float lcos = dot(ray.dir, origin - ray.origin);
		float tmp = lcos*lcos + radius*radius - l*l;
		assert(abs(norm(ray.dir)-1) < 3e-6);
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
		return ray.origin + ray.dir * resl;
	}

	vec3 normalAtPoint(point p)
	{
#ifdef DEBUG
		assert(abs(norm(p - origin) - radius) < 1e-4);
#endif
		return normalize(p - origin);
	}
};


struct Volume
{
};


struct triangle
{
	vec3 v1,v2,v3;
	vec3 vn1, vn2, vn3;

	vec3 planeNormal;
	mat3 interpMatrix;

	void preprocess() 
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
		// std::cout << planeNormal << std::endl;
		// make sure plane normal points outward
		if (dot(planeNormal, vn1) < 0)
			planeNormal *= -1;
		interpMatrix = mat3(vn1,vn2,vn3) * inverse(mat3(v1,v2,v3));
	}

	bool hasIntersection(Ray r)
	{
		// TODO temporarily using a slow method.
		// applying matrix transformation should be faster
		float c1 = det(mat3(r.dir, v2-v1, v3-v1));
		if (c1 == 0) return false;
		float c0 = det(mat3(r.origin - v1, v2-v1, v3-v1));
		float k = -c0 / c1;
		if (k <= 0) return false;
		// NOTE: be careful to prevent a surface emits ray hitting itself
		// TODO this is SUPER SLOW!
		point p = r.origin + (-c0/c1) * r.dir;
		vec3 vd1 = cross(p-v1, v2-v1);
		vec3 vd2 = cross(p-v2, v3-v2);
		vec3 vd3 = cross(p-v3, v1-v3);
		float nvd1 = norm(vd1);
		float nvd2 = norm(vd2);
		float nvd3 = norm(vd3);
		if (nvd1 > 1e-5 && nvd2 > 1e-5 && dot(vd1,vd2) < 0) return false;
		if (nvd1 > 1e-5 && nvd3 > 1e-5 && dot(vd1,vd3) < 0) return false;
		if (nvd3 > 1e-5 && nvd2 > 1e-5 && dot(vd3,vd2) < 0) return false;
		return true;
	}

	point intersection(Ray r)
	{
		float c1 = det(mat3(r.dir, v2-v1, v3-v1));
		float c0 = det(mat3(r.origin - v1, v2-v1, v3-v1));
		// c1=0: ray parallel to / coincide with plane
		return r.origin + (-c0/c1) * r.dir;
		// TODO WARNING: possibly returnval contains NAN / INF
	}

	vec3 interpolatedNormal(point p)
	{
#ifdef DEBUG
		assert(abs(dot(p-v1, planeNormal)) < 1e-5);
#endif
		// simulate flat shading
		return planeNormal;
		// real interpolated normal:
		// return interpMatrix * p;
	}
};



#endif
