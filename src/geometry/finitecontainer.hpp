#pragma once

#include <vector>
#include "geometry.hpp"

struct FiniteContainer: public FinitePrimitive
{
public:
	FiniteContainer(const std::vector<FinitePrimitive*>& list)
	{

	}

	bool intersect(const Ray& ray, point* result) const
	{
		assert(false);
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
		assert(false);
	}
};
