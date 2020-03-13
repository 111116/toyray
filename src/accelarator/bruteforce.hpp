#pragma once

#include "accelarator.hpp"

struct Bruteforce : public Accelarator {
	std::vector<std::pair<Primitive*, Object*>> list;
	Bruteforce(const std::vector<Object*>& list) {
		std::cout << "building Bruteforce of " << list.size() << " objects" << std::endl;
		for (Object* o: list) {
			this->list.push_back({o->primitive, o});
		}
	}

	HitInfo hit(const Ray& ray)
	{
		HitInfo hit;
		float sqrdist;
		Primitive::Hit res;
		// bruteforcing checking against every primitive
		for (auto o: list) {
			if (o.first->intersect(ray, &res)) {
				if (!hit || sqrdist > sqrlen(res.p - ray.origin)) {
					sqrdist = sqrlen(res.p - ray.origin);
					hit = HitInfo(res, o.second);
				}
			}
		}
		return hit;
	}

};
