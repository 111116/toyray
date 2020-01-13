#pragma once

#include "color.h"

struct Light {
	virtual Color radiance(const Ray& shadowray) = 0;
};

struct DiffuseAreaLight: public Light {
	Color color;
	DiffuseAreaLight(Color color): color(color) {
	}
	Color radiance(const Ray& shadowray) {
		return color;
	}
};