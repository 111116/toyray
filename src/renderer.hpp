#pragma once

#include <vector>

#include "object.hpp"
#include "lights/light.hpp"
#include "accelarator/accelarator.hpp"
#include "cameras/camera.hpp"
#include "image.hpp"
#include <functional>

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
	Color normal(Ray, Sampler&);
	Color albedo(Ray, Sampler&);
	Color radiance(Ray, Sampler&);
	Image render(decltype(&Renderer::radiance) func, bool reportProgress = true);
private:
	Color renderPixel(std::function<Color(Ray, Sampler&)> func, const vec2f& pixelpos, const vec2f& pixelsize);
};
