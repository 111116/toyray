#pragma once

#include "accelarator.hpp"

struct Bruteforce : public Accelarator {
	std::vector<std::pair<Primitive*, Object*>> list;
	Bruteforce(const std::vector<Object*>& list) {
		for (Object* o: list) {
			for (Primitive* p: o->mesh->faces)
				this->list.push_back({p, o});
		}
	}

	HitInfo hit(const Ray& ray)
	{
		HitInfo hit = {NULL, NULL};
		float dist;
		point res;
		// bruteforcing checking against every primitive
		for (auto o: list) {
			if (o.first->intersect(ray, &res)) {
				if (!hit || dist > norm(res - ray.origin)) {
					hit = {o.first, o.second, res};
					dist = norm(res - ray.origin);
				}
			}
		}
		return hit;
	}

};