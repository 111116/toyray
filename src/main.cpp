// naive path tracing implementation


#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>
#include "lightprobe.hpp"
#include "color.h"
// #include "writebmp.h"
#include "lib/saveexr.h"
#include "jsonutil.hpp"
#include "env.hpp" // model directory
#include "object.hpp"
#include "cameras/camera.hpp"
#include "accelarator/bruteforce.hpp"
#include "accelarator/bvhsah.hpp"
#ifdef THREADED
#include <omp.h>
#endif

#define DEBUG


float* pixels;
int nspp = 4; // default number of samples per pixel, may be overrided in parameters

std::vector<Object*> objects;
Accelarator* acc;
std::unordered_map<std::string, BSDF*> bsdf;
std::string outfilename;
int max_bounces;

// Color albedo(Ray ray) {
// 	HitInfo hit = acc->hit(ray);
// 	if (!hit) {
// 		return Color();
// 	}
// 	return hit.object->bsdf->albedo;
// }

Color normal(Ray ray) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		return Color();
	}
	return Color(0.5,0.5,0.5) + 0.5 * hit.primitive->Ns(hit.p);
}

// pointers to all objects with emission, for light sampling
std::vector<Object*> samplable_light_objects;
LightProbe* globalLightProbe = NULL;

// sample radiance using PT, with light/importance sampling
Color brightness(Ray ray, int depth = 0, bool reject_samplable_light = false) {
	if (depth > max_bounces)
		return Color();
	ray.origin += 1e-3 * ray.dir;
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		if (globalLightProbe) return globalLightProbe->radiance(ray.dir);
		return Color();
	}
	auto Ns = hit.primitive->Ns(hit.p);
	auto Ng = hit.primitive->Ng(hit.p);
	if (hit.object->emission) {
		if (reject_samplable_light && hit.object->samplable) return Color();
		return hit.object->emission->radiance(ray);
	}

	Color result;
	// sample direct light
	if (!samplable_light_objects.empty())
	{
		Object* light = samplable_light_objects[rand() % samplable_light_objects.size()];
		float pdf;
		Primitive* shape;
		point lightp = light->sample_point(pdf, shape);
		// check if direct light was blocked
		if (pdf > 1e-8) {
			Ray shadowray = {hit.p, normalize(lightp - hit.p)};
			shadowray.origin += 1e-3 * shadowray.dir;
			HitInfo shadowhit = acc->hit(shadowray);
			if (!shadowhit || norm(shadowhit.p - hit.p) >= norm(lightp - hit.p) - 1e-3) {
				pdf /= samplable_light_objects.size();
				vec3 lightN = shape->Ns(lightp);
				float dw = pow(norm(lightp - hit.p), -2) * fabs(dot(shadowray.dir, lightN));
				result = hit.object->bsdf->f(-ray.dir, shadowray.dir, Ns, Ng) * fabs(dot(Ns, shadowray.dir)) * light->emission->radiance(shadowray) * (dw / pdf);
			}
		}
	}
	vec3 wi;
	float pdf;
	Color f = hit.object->bsdf->sample_f(-ray.dir, wi, Ns, Ng, pdf);
	if (pdf > 1e-8) {
		Ray r = {hit.p, wi};
		return result + 1/pdf * f * fabs(dot(Ns, r.dir)) * brightness(r, depth+1, true);
	}
	return Color();
}


int main(int argc, char* argv[])
{
#ifdef DEBUG
	fprintf(stderr, "WARNING: running in debug mode.\n");
#endif
#ifdef THREADED
#pragma omp parallel
	{
	#pragma omp single
		std::cerr << omp_get_num_threads() << " THREADS" << std::endl;
	}
#else
	std::cerr << "Threading disabled" << std::endl;
#endif
	if (argc<=1) {
		std::cerr << "Usage: " << argv[0] << " conf.json [nspp]"<< std::endl;
		return 1;
	}
	std::ifstream fin(argv[1]);
	modelpath = directoryOf(argv[1]);
	// load scene conf file
	Json conf;
	fin >> conf;
	for (auto o: conf["bsdfs"]) {
		bsdf[o["name"]] = newMaterial(o);
	}
#pragma omp parallel for schedule(dynamic)
	for (auto o: conf["primitives"]) {
		if (o["type"] == "infinite_sphere") {
			// assume infinite_sphere will only be used for light probes
			globalLightProbe = new LightProbe(getpath(o["emission"]).c_str());
			// TODO currently light probes are not samplable
		}
		else {
			Object* newobj;
			newobj = new Object(o, bsdf[o["bsdf"]]);
#pragma omp critical
			objects.push_back(newobj);
		}
	}
	for (auto o: objects) {
		if (o->emission && o->samplable)
			samplable_light_objects.push_back(o);
	}
	acc = new BVH(objects);
	Camera* camera = new PinholeCamera(conf["camera"]);
	assert(conf["integrator"]["type"] == "path_tracer");
	assert(conf["integrator"]["enable_light_sampling"] == true);
	max_bounces = conf["integrator"]["max_bounces"];
	nspp = conf["renderer"]["spp"];
	if (argc>2) nspp = atoi(argv[2]);
	outfilename = conf["renderer"]["hdr_output_file"];

	// prepare film
	pixels = new float[camera->resolution_x * camera->resolution_y * 3];
	fprintf(stderr, "INFO: rendering at %d x %d x %d spp\n", camera->resolution_x, camera->resolution_y, nspp);
	// start rendering
	int line_finished = 0;
	auto start = std::chrono::system_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int y=0; y<camera->resolution_y; ++y)
	{
		for (int x=0; x<camera->resolution_x; ++x)
		{
			Color res;
			for (int i=0; i<nspp; ++i)
			{
				// manual camera setup
				float w = 4;
				float u = (x+randf()) / camera->resolution_x;
				float v = (y+randf()) / camera->resolution_y;
				Ray ray = camera->sampleray(u,v);
				Color tres = brightness(ray);
				if (norm(tres)<1e8) res += tres;
			}
			res *= 1.0/nspp;
			int pxid = y*camera->resolution_x + x;
			pixels[3*pxid+0] = res.x;
			pixels[3*pxid+1] = res.y;
			pixels[3*pxid+2] = res.z;
		}
		#pragma omp critical
		fprintf(stderr, "\r%.1f%%", 100.0f*(++line_finished)/camera->resolution_y);
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "  " << elapsed_seconds.count() << "s\n";
	fprintf(stderr, "INFO: Writing result to %s\n", outfilename.c_str());
	SaveEXR(pixels, camera->resolution_x, camera->resolution_y, outfilename.c_str());
}
