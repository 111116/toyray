#pragma once

#include <vector>
#include "../geometry/geometry.hpp"

struct RawHitInfo {
	Primitive* primitive = NULL;
	point p;
	operator bool() {
		return primitive!=NULL;
	}
	RawHitInfo(){}
	RawHitInfo(Primitive* primitive, point p):
		primitive(primitive), p(p)
	{}
};

struct RawBVHSAH {
private:
	struct treenode {
		treenode* lc = NULL;
		treenode* rc = NULL;
		Primitive* shape = NULL;
		AABox bound;
	};
	treenode* root = NULL;

	void build(std::vector<Primitive*> list, treenode*& cur)
	{
		// create tree node
		if (!cur) {
			cur = new treenode();
		}
		// bind shape to leaf nodes
		if (list.size() == 1)
		{
			cur->shape = list[0];
			cur->bound = list[0]->boundingVolume();
			return;
		}
		// compute bounding box
		AABox& bound = cur->bound;
		bound = list[0]->boundingVolume();
		for (int i=1; i<list.size(); ++i) {
			bound = bound + list[i]->boundingVolume();
		}
		// sort in sparsest dimension
		auto cmp = +[](Primitive* a, Primitive* b) {
			return a->boundingVolume().x1 < b->boundingVolume().x1;
		};
		if (bound.y2 - bound.y1 > bound.x2 - bound.x1) {
			cmp = +[](Primitive* a, Primitive* b) {
				return a->boundingVolume().y1 < b->boundingVolume().y1;
			};
		}
		if (bound.z2 - bound.z1 > bound.x2 - bound.x1 &&
			bound.z2 - bound.z1 > bound.y2 - bound.y1) {
			cmp = +[](Primitive* a, Primitive* b) {
				return a->boundingVolume().z1 < b->boundingVolume().z1;
			};
		}
		std::sort(list.begin(), list.end(), cmp);
		// determine split position to minimize sum of surface area of bounding boxes
		std::vector<float> prefix_area, suffix_area;
		prefix_area.resize(list.size());
		suffix_area.resize(list.size());
		AABox accu = list[0]->boundingVolume();
		for (int i=0; i<list.size(); ++i) {
			accu = accu + list[i]->boundingVolume();
			prefix_area[i] = accu.surfaceArea();
		}
		accu = list[list.size()-1]->boundingVolume();
		for (int i=list.size()-1; i>=0; --i) {
			accu = accu + list[i]->boundingVolume();
			suffix_area[i] = accu.surfaceArea();
		}
		int rangemin = round(list.size()*0.1);
		int rangemax = round(list.size()*0.9)-1;
		// limit split point from being too close to side
		int best = rangemin;
		for (int i=rangemin; i<rangemax; ++i) {
			if (prefix_area[i] + suffix_area[i+1] < prefix_area[best] + suffix_area[best+1])
				best = i;
		}
		// recursive partition
		build(std::vector<Primitive*>(list.begin(), list.begin()+best+1), cur->lc);
		build(std::vector<Primitive*>(list.begin()+best+1, list.end()), cur->rc);
	}

	std::vector<Primitive*> list;

	RawHitInfo treehit(const Ray& ray, treenode* node) {
		if (node == NULL) return RawHitInfo();
		if (!node->bound.intersect(ray)) return RawHitInfo();
		if (node->shape != NULL) {
			RawHitInfo hit;
			point res;
			if (node->shape->intersect(ray, &res)) {
				hit = RawHitInfo(node->shape, res);
			}
			return hit;
		}
		RawHitInfo resl = treehit(ray, node->lc);
		if (!resl) return treehit(ray, node->rc);
		RawHitInfo resr = treehit(ray, node->rc);
		return (!resr || sqrlen(resl.p - ray.origin) < sqrlen(resr.p - ray.origin))? resl: resr;
	}

public:
	RawBVHSAH(const std::vector<Primitive*>& list) {
		std::cout << "building SAH BVH of " << list.size() << " primitives" << std::endl;
		build(list, root);
	}

	RawHitInfo hit(const Ray& ray)
	{
		return treehit(ray, root);
	}
};
