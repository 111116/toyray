#pragma once

#include "geometry.hpp"
#include "transform.hpp"

#include "triangle.hpp" // DEBUG

class Quad: public BasicPrimitive, public SurfaceSamplablePrimitive
{
	const mat4f invTrans;
	float one_by_area;
	vec3f normal, v1, v2, v3, v4;
	Triangle* t;
public:

	Quad(const mat4f& transform): invTrans(inverse(transform))
	{
		_internalTransform = true;
		// calculate area
        v1 = vec3f(-0.5f, 0.0f, -0.5f);
        v2 = vec3f( 0.5f, 0.0f, -0.5f);
        v3 = vec3f( 0.5f, 0.0f,  0.5f);
        v4 = vec3f(-0.5f, 0.0f,  0.5f);
        v1 = Transformed::transformedPoint(transform, v1);
        v2 = Transformed::transformedPoint(transform, v2);
        v3 = Transformed::transformedPoint(transform, v3);
        v4 = Transformed::transformedPoint(transform, v4);
        std::cout << "QUAD of" << std::endl;
        std::cout << "    " << v1 << "\n";
        std::cout << "    " << v2 << "\n";
        std::cout << "    " << v3 << "\n";
        std::cout << "    " << v4 << "\n";
        vec2f t2;
        vec3f t3;
        t = new Triangle(v1,v2,v3,t2,t2,t2,t3,t3,t3);
        t->recompute_normal();
        normal = normalized(cross(v1-v2, v3-v2));
        one_by_area = 1.0 / fabs(norm(cross(v1-v2, v3-v2)));
	}

	bool intersect(const Ray& ray, float& result) const
	{
		return t->intersect(ray,result);
		Ray iray(Transformed::transformedPoint(invTrans, ray.origin),
			(invTrans * vec4f(ray.dir,0)).xyz());
		float t = -iray.origin.y / iray.dir.y;
		point p = iray.atParam(t);
		if (t>0 && p.x>-0.5 && p.x<0.5 && p.z>-0.5 && p.z<0.5)
		{
			result = t;
			return true;
		}
		return false;
	}

	vec3f Ns(const point& p) const
	{
		// return t->Ns(p);
		return normal;
	}

	vec3f Ng(const point& p) const
	{
		// return t->Ns(p);
		return normal;
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		return t->sampleSurface(sampler);
		vec2f t = sampler.get2f();
		point p = v2 + t.x * (v1-v2) + t.y * (v3-v2);
		return SampleInfo(p, normal, one_by_area);
	}

	AABox boundingVolume() const
	{
		// return t->boundingVolume();
		AABox v;
		v.x1 = std::min(v1.x, std::min(v2.x, std::min(v3.x, v4.x)));
		v.x2 = std::max(v1.x, std::max(v2.x, std::max(v3.x, v4.x)));
		v.y1 = std::min(v1.y, std::min(v2.y, std::min(v3.y, v4.y)));
		v.y2 = std::max(v1.y, std::max(v2.y, std::max(v3.y, v4.y)));
		v.z1 = std::min(v1.z, std::min(v2.z, std::min(v3.z, v4.z)));
		v.z2 = std::max(v1.z, std::max(v2.z, std::max(v3.z, v4.z)));
		return v;
	}

};