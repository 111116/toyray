#ifndef BVH_H
#define BVH_H

#include "geometry.h"

class BVH
{
public:
	void build();
	intersect(Ray ray);
};

#endif
