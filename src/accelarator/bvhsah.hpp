#pragma once

#include "accelarator.hpp"
#include "lib/consolelog.hpp"
#include "bsdfs/transparency.hpp"

struct BVH : public Accelarator {
private:
	struct treenode {
		treenode* lc = NULL;
		treenode* rc = NULL;
		Primitive* shape = NULL;
		Object* object = NULL;
		AABox bound;
	};
	treenode* root = NULL;

	void build(std::vector<std::pair<Primitive*, Object*>> list, treenode*& cur)
	{
		// create tree node
		if (!cur) {
			cur = new treenode();
		}
		// bind shape to leaf nodes
		if (list.size() == 1)
		{
			cur->shape = list[0].first;
			cur->object = list[0].second;
			cur->bound = list[0].first->boundingVolume();
			return;
		}
		// compute bounding box
		AABox& bound = cur->bound;
		bound = list[0].first->boundingVolume();
		for (int i=1; i<list.size(); ++i) {
			bound = bound + list[i].first->boundingVolume();
		}
		// sort in sparsest dimension
		auto cmp = +[](const std::pair<Primitive*, Object*>& a, const std::pair<Primitive*, Object*>& b) {
			return a.first->boundingVolume().x1 < b.first->boundingVolume().x1;
		};
		if (bound.y2 - bound.y1 > bound.x2 - bound.x1) {
			cmp = +[](const std::pair<Primitive*, Object*>& a, const std::pair<Primitive*, Object*>& b) {
				return a.first->boundingVolume().y1 < b.first->boundingVolume().y1;
			};
		}
		if (bound.z2 - bound.z1 > bound.x2 - bound.x1 &&
			bound.z2 - bound.z1 > bound.y2 - bound.y1) {
			cmp = +[](const std::pair<Primitive*, Object*>& a, const std::pair<Primitive*, Object*>& b) {
				return a.first->boundingVolume().z1 < b.first->boundingVolume().z1;
			};
		}
		std::sort(list.begin(), list.end(), cmp);
		// determine split position to minimize sum of surface area of bounding boxes
		std::vector<float> prefix_area, suffix_area;
		prefix_area.resize(list.size());
		suffix_area.resize(list.size());
		AABox accu = list[0].first->boundingVolume();
		for (int i=0; i<list.size(); ++i) {
			accu = accu + list[i].first->boundingVolume();
			prefix_area[i] = accu.surfaceArea();
		}
		accu = list[list.size()-1].first->boundingVolume();
		for (int i=list.size()-1; i>=0; --i) {
			accu = accu + list[i].first->boundingVolume();
			suffix_area[i] = accu.surfaceArea();
		}
		int rangemin = round(list.size()*0.15);
		int rangemax = round(list.size()*0.85)-1;
		// limit split point from being too close to side
		int best = rangemin;
		for (int i=rangemin; i<rangemax; ++i) {
			if (prefix_area[i] + suffix_area[i+1] < prefix_area[best] + suffix_area[best+1])
				best = i;
		}
		// recursive partition
		build(std::vector<std::pair<Primitive*, Object*>>(list.begin(), list.begin()+best+1), cur->lc);
		build(std::vector<std::pair<Primitive*, Object*>>(list.begin()+best+1, list.end()), cur->rc);
	}

	std::vector<std::pair<Primitive*, Object*>> list;

	// WARN: this function doesn't guarantee that *h is unchanged if not hit
	static inline bool opaqueHit(treenode* node, Ray ray, Primitive::Hit* h)
	{
		if (node->shape->intersect(ray, h))
		{
			Transparency* bsdf = dynamic_cast<Transparency*>(node->object->bsdf);
			if (bsdf == NULL || !bsdf->isTransparent(h->uv))
				return true;
			do {
				ray.origin = h->p + 1e-2 * ray.dir;
			}
			while (node->shape->intersect(ray, h) && bsdf->isTransparent(h->uv));
			return !bsdf->isTransparent(h->uv);
		}
		return false;
	}

	HitInfo treehit(const Ray& ray, treenode* node) {
		if (node == NULL) return HitInfo();
		if (!node->bound.intersect(ray)) return HitInfo();
		if (node->shape != NULL) {
			HitInfo hit;
			Primitive::Hit h;
			if (opaqueHit(node, ray, &h)) {
				hit = HitInfo(h, node->object);
			}
			return hit;
		}
		HitInfo resl = treehit(ray, node->lc);
		if (!resl) return treehit(ray, node->rc);
		HitInfo resr = treehit(ray, node->rc);
		return (!resr || sqrlen(resl.p - ray.origin) < sqrlen(resr.p - ray.origin))? resl: resr;
	}

	void printSA(treenode* node, int depth = 0) {
		if (node == NULL) return;
		for (int i=0; i<depth; ++i)
			std::cout << "| ";
		std::cout << node->bound.surfaceArea() << std::endl;
		printSA(node->lc, depth+1);
		printSA(node->rc, depth+1);
		delete node;
	}

public:
	BVH(const std::vector<Object*>& list) {
		console.log("building SAH BVH of", list.size(), "objects");
		if (list.size() == 0) throw "BVH: Object list empty!";
		for (Object* o: list) {
			this->list.push_back({o->primitive, o});
		}
		build(this->list, root);
		// printSA(root);
	}

	HitInfo hit(const Ray& ray)
	{
		return treehit(ray, root);
	}

};
