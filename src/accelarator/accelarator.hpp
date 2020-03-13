#pragma once

#include "../object.hpp"
#include "../geometry/geometry.hpp"

class HitInfo : public Primitive::Hit {
public:
	Object const* object = NULL;
	HitInfo(){}
	HitInfo(const Primitive::Hit& hit, Object const* object):
		Primitive::Hit(hit), object(object) {}
};

struct Accelarator {
	virtual HitInfo hit(const Ray& ray) = 0;
};
