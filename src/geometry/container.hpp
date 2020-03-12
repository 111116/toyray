#pragma once

#include <vector>
#include "geometry.hpp"
#include "../accelarator/bvh.hpp"

struct FiniteContainer: public Primitive
{
	RawBVHSAH* bvh;
	AABox bound;
public:
	FiniteContainer(const std::vector<Primitive*>& list)
	{
		bvh = new RawBVHSAH(list);
		bound = list[0]->boundingVolume();
		for (int i=1; i<list.size(); ++i) {
			bound = bound + list[i]->boundingVolume();
		}
	}

	bool intersect(const Ray& ray, point* result) const
	{
		auto h = bvh->hit(ray);
		if (h) *result = h.p;
		return h;
	}
	 
	vec3f Ns(point p) const
	{
		assert(false);
	}

	vec3f Ng(point p) const
	{
		assert(false);
	}

	AABox boundingVolume() const
	{
		return bound;
	}
};
