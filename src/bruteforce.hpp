#pragma once

#include "object.hpp"
#include "geometry.h"

struct Bruteforce {
	std::vector<std::pair<Primitive*, Object*>> list;
	Bruteforce(const std::vector<Object*>& list) {
		for (Object* o: list) {
			for (Primitive* p: o->mesh->faces)
				this->list.push_back({p, o});
		}
	}

	std::pair<Primitive*, Object*> hit(const Ray& ray)
	{
		std::pair<Primitive*, Object*> hit = {NULL, NULL};
		float dist;
		point res;
		// bruteforcing checking against every primitive
		for (auto o: list) {
			if (o.first->intersect(ray, &res)) {
				if (hit.first == NULL || dist > norm(res - ray.origin)) {
					hit = o;
					dist = norm(res - ray.origin);
				}
			}
		}
		return hit;
	}

};