#pragma once

#include "../object.hpp"
#include "../geometry/geometry.hpp"

struct HitInfo {
	BasicPrimitive const* primitive = NULL;
	Object* object = NULL;
	point p;
	operator bool() {
		return primitive!=NULL;
	}
	HitInfo(){}
	HitInfo(BasicPrimitive const* primitive, Object* object, point p):
		primitive(primitive), object(object), p(p)
	{}
};

struct Accelarator {
	virtual HitInfo hit(const Ray& ray) = 0;
};
