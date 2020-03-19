#pragma once

#include <vector>

#include "object.hpp"
#include "lights/light.hpp"
#include "accelarator/accelarator.hpp"
#include "cameras/camera.hpp"

class Renderer
{
	// TODO don't use fixed eps
	const float geoEPS = 1e-3;
public:
	int max_bounces = 16;
	int nspp = 1;

	std::vector<Object*> objects;
	std::vector<Light*> samplable_lights;
	std::vector<Light*> global_lights;
	Accelarator* acc;
	Camera* camera;

	// assume rendered pixel by pixel
	Color render(const vec2f& pixelpos, const vec2f& pixelsize);

private:
	Color normal(Ray, Sampler&);
	Color radiance(Ray, Sampler&);

};
