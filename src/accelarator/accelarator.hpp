#pragma once

#include "../object.hpp"
#include "../geometry.h"

struct HitInfo {
	Primitive* primitive = NULL;
	Object* object = NULL;
	point p;
	operator bool() {
		return primitive!=NULL;
	}
	HitInfo(){}
	HitInfo(Primitive* primitive, Object* object, point p):
		primitive(primitive), object(object), p(p)
	{}
};

struct Accelarator {
	virtual HitInfo hit(const Ray& ray) = 0;
};
