#ifndef BVH_H
#define BVH_H

#include "geometry.h"

class BVH
{
private:
	struct node
	{
		node* lc = NULL;
		node* rc = NULL;
		Primitive* shape = NULL;
		AABox bound;
	};
	node* root = NULL;
	
public:
	void build(std::vector<Primitive*> list, node*& cur = root)
	{
		if (list.size() == 1)
		{
			shape = list[0];
			return;
		}
		
	}

	bool intersect(Ray ray, point* p, node* cur = root)
	{
		if (cur == NULL) return false;
		if (shape) return shape->intersect(ray, p);
		if (!bound.intersect(ray)) return false;
		point pl;
		if (intersect(ray, &pl, lc))
		{
			point pr;
			if (intersect(ray, &pr, rc) && norm(pr-ray.origin) < norm(pl-ray.origin))
				p = pr;
			else
				p = pl;
			return true;
		}
		else
			return intersect(ray, p, rc);
	}
};

#endif
