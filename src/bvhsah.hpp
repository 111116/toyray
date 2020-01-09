#pragma once

#include "object.hpp"
#include "geometry.h"

struct BVH {

	struct node {
		node* lc = NULL;
		node* rc = NULL;
		Primitive* shape = NULL;
		AABox bound;
	};

	void build(std::vector<Primitive*> list, node*& cur)
	{
		if (!cur) cur = new node();
		if (list.size() == 1)
		{
			cur->shape = list[0];
			cur->bound = list[0]->boundingVolume();
			return;
		}
		AABox& bound = cur->bound;
		bound = list[0]->boundingVolume();
		for (int i=1; i<list.size(); ++i) {
			bound = bound + list[i]->boundingVolume();
		}
		auto cmp = +[](const Primitive* a, const Primitive* b) {
			return a->boundingVolume().x1 < b->boundingVolume().x1;
		};
		if (bound.y2 - bound.y1 > bound.x2 - bound.x1) {
			cmp = +[](const Primitive* a, const Primitive* b) {
				return a->boundingVolume().y1 < b->boundingVolume().y1;
			};
		}
		if (bound.z2 - bound.z1 > bound.x2 - bound.x1 &&
			bound.z2 - bound.z1 > bound.y2 - bound.y1) {
			cmp = +[](const Primitive* a, const Primitive* b) {
				return a->boundingVolume().z1 < b->boundingVolume().z1;
			};
		}
		std::sort(list.begin(), list.end(), cmp);
		std::vector<float> prefix_area, suffix_area;
		prefix_area.resize(list.size());
		suffix_area.resize(list.size());
		AABox cur = list[0].boundingVolume();
		for (int i=0; i<list.size(); ++i) {
			cur = cur + list[i].boundingVolume();
			prefix_area[i] = cur.surfaceArea();
		}
		cur = list[list.size()-1].boundingVolume();
		for (int i=list.size()-1; i>=0; --i) {
			cur = cur + list[i].boundingVolume();
			suffix_area[i] = cur.surfaceArea();
		}
		
	}

	std::vector<std::pair<Primitive*, Object*>> list;
	BVH(const std::vector<Object*>& list) {
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