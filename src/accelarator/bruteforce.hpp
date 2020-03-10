#pragma once

#include "accelarator.hpp"

struct Bruteforce : public Accelarator {
	std::vector<std::pair<Primitive*, Object*>> list;
	Bruteforce(const std::vector<Object*>& list) {
		for (Object* o: list) {
			if (o->container) {
				for (Primitive* p: o->container->faces)
					this->list.push_back({p, o});
			}
			else {
				this->list.push_back({o->primitive, o});
			}
		}
	}

	HitInfo hit(const Ray& ray)
	{
		HitInfo hit;
		float dist;
		point res;
		// bruteforcing checking against every primitive
		for (auto o: list) {
			if (o.first->intersect(ray, &res)) {
				if (!hit || dist > norm(res - ray.origin)) {
					hit = HitInfo(o.first, o.second, res);
					dist = norm(res - ray.origin);
				}
			}
		}
		return hit;
	}

};
