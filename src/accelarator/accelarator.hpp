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
};

struct Accelarator {
	virtual HitInfo hit(const Ray& ray) = 0;
};