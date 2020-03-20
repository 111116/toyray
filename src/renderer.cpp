
#include "renderer.hpp"
#include "samplers/mt19937sampler.hpp"


Color Renderer::render(const vec2f& pixelpos, const vec2f& pixelsize) {
	Color res;
	for (int i=0; i<nspp; ++i) {
		Sampler* sampler = new MT19937Sampler(std::hash<double>{}(pixelpos.x*PI+pixelpos.y), i);
		vec2f uv = pixelpos + sampler->get2f() * pixelsize;
		Ray ray = camera->sampleray(uv);
		Color tres = radiance(ray, *sampler);
		/*if (norm(tres)<1e8)*/ res += tres;
	}
	return res / nspp;
}


Color Renderer::normal(Ray ray, Sampler&) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		return Color();
	}
	return Color(0.5,0.5,0.5) + 0.5 * hit.Ns;
}


// path tracer
Color Renderer::radiance(Ray ray, Sampler& sampler)
{
	Color through(1);
	Color result;
	bool lastDirac = true; // accept all light sources if directly into camera
	for (int nbounce = 0; nbounce <= max_bounces; nbounce++)
	{
		HitInfo hit = acc->hit(ray);
		if (!hit) {
			for (Light* l: global_lights)
				result += through * l->radiance(ray);
			break;
		}
		if (hit.object->emission) {
			// TODO
			if (lastDirac || !hit.object->emission->samplable)
				result += through * hit.object->emission->radiance(ray);
		}
		BSDF* bsdf = hit.object->bsdf;
		if (!bsdf) break;
		// direct light (local) // TODO
		for (Light* l: samplable_lights) {
			vec3f dirToLight;
			float dist;
			Color irr = l->sampleIrradiance(hit.p, dirToLight, dist, sampler);
			Color f = bsdf->f(-ray.dir, dirToLight, hit.Ns, hit.Ng);
			if (f == vec3f(0)) continue;
			dist -= geoEPS; // cancel amount that ray origin is moved forward
			// shadow ray test
			Ray shadowray(hit.p + geoEPS * dirToLight, dirToLight);
			HitInfo shadowhit = acc->hit(shadowray);
			if (!shadowhit || norm(shadowhit.p - shadowray.origin) > (1-geoEPS) * dist)
				result += through * irr * f * fabs(dot(hit.Ns, dirToLight));
		}
		// indirect light (bsdf importance sampling)
		vec3f newdir;
		Color f = bsdf->sample_f(-ray.dir, newdir, hit.Ns, hit.Ng, lastDirac, sampler); // already scaled by 1/pdf
		through *= fabs(dot(newdir, hit.Ns)) * f;
		if (through == vec3f() || !(sqrlen(through) < 1e20)) break;
		ray = Ray(hit.p + geoEPS*newdir, newdir);
	}
	return result;
}
