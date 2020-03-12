#pragma once

#include <vector>
#include "geometry.hpp"
#include "../accelarator/bvh.hpp"

class FiniteContainer: public Primitive
{
	RawBVHSAH* bvh;
	AABox bound;
protected:
	std::vector<Primitive*> list;
public:
	FiniteContainer(const std::vector<Primitive*>& list): list(list)
	{
		bvh = new RawBVHSAH(list);
		bound = list[0]->boundingVolume();
		for (int i=1; i<list.size(); ++i) {
			bound = bound + list[i]->boundingVolume();
		}
	}

	bool intersect(const Ray& ray, Hit* result) const
	{
		return *result = bvh->hit(ray);
	}
	 
	AABox boundingVolume() const
	{
		return bound;
	}
};
