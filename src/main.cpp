// ray casting

#define DEBUG

#ifdef DEBUG
#include <assert.h>
#endif

#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>
// #include "lightprobe.hpp"
#include "color.h"
// #include "writebmp.h"
#include "lib/saveexr.h"
#include "jsonutil.hpp"
#include "env.hpp" // model directory
#include "object.hpp"
#include "cameras/camera.hpp"
#include "accelarator/bvhsah.hpp"
#include "accelarator/bruteforce.hpp"
#include "samplers/randomsampler.hpp"
#include "film.hpp"

#ifdef THREADED
#include <omp.h>
#endif


std::vector<Object*> objects;
// pointers to all objects with emission, for light sampling
std::vector<Object*> samplable_light_objects;
// LightProbe* globalLightProbe = NULL;
Accelarator* acc;
std::unordered_map<std::string, BSDF*> bsdf;
// int max_bounces;


Color normal(Ray ray) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		return Color();
	}
	return Color(0.5,0.5,0.5) + 0.5 * hit.primitive->Ns(hit.p);
}


// sample radiance using ray casting
Color brightness(Ray ray) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		// if (globalLightProbe) result += lambda * globalLightProbe->radiance(ray.dir);
		return Color(0);
	}
	auto Ns = hit.primitive->Ns(hit.p);
	auto Ng = hit.primitive->Ng(hit.p);
	Color result;
	if (hit.object->emission) {
		result += hit.object->emission->radiance(ray);
	}
	BSDF* bsdf = hit.object->bsdf;
	if (!bsdf) return result;
	// direct light (local)
	// if (!samplable_light_objects.empty())
	// {
	// 	Object* light = samplable_light_objects[rand() % samplable_light_objects.size()];
	// 	float pdf;
	// 	Primitive* shape;
	// 	point lightp = light->sample_point(pdf, shape);
	// 	// check if direct light was blocked
	// 	if (pdf > 1e-8) {
	// 		Ray shadowray = {hit.p, normalized(lightp - hit.p)};
	// 		shadowray.origin += 1e-3 * shadowray.dir;
	// 		pdf /= samplable_light_objects.size();
	// 		vec3f lightN = shape->Ns(lightp);
	// 		float dw = pow(norm(lightp - hit.p), -2) * fabs(dot(shadowray.dir, lightN));
	// 		result += bsdf->f(-ray.dir, shadowray.dir, Ns, Ng) * fabs(dot(Ns, shadowray.dir)) * light->emission->radiance(shadowray) * (dw / pdf);
	// 	}
	// }
	// patch for Phong ambient
	if (bsdf->component)
	{
		Phong* phong = dynamic_cast<Phong*>(bsdf->component);
		if (phong != NULL) {
			result += phong->Kd * dot(Ns, vec3f(0,1,0));
		}
	}
	return result;
}


int nspp = 1; // may be overriden in conf


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
	// parse commandline args
	if (argc<=1) {
		std::cerr << "Usage: " << argv[0] << " <json file>"<< std::endl;
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
		// if (o["type"] == "infinite_sphere") {
		// 	// assume infinite_sphere will only be used for light probes
		// 	globalLightProbe = new LightProbe(getpath(o["emission"]).c_str());
		// 	// TODO currently light probes are not samplable
		// }
		// else {
			Object* newobj;
			newobj = new Object(o, bsdf[o["bsdf"]]);
#pragma omp critical
			objects.push_back(newobj);
		// }
	}
	for (auto o: objects) {
		if (o->emission && o->samplable)
			samplable_light_objects.push_back(o);
	}
	acc = new Bruteforce(objects);
	Camera* camera = newCamera(conf["camera"]);
	nspp = conf["renderer"]["spp"];

	// prepare film
	Film film(camera->resx, camera->resy);
	fprintf(stderr, "INFO: rendering at %d x %d x %d spp\n", camera->resx, camera->resy, nspp);
	// start rendering
	auto start = std::chrono::system_clock::now();
	int line_finished = 0;
#pragma omp parallel for schedule(dynamic)
	for (int y=0; y<camera->resy; ++y)
	{
		for (int x=0; x<camera->resx; ++x)
		{
			Color res;
			for (int i=0; i<nspp; ++i)
			{
				Sampler* sampler = new RandomSampler();
				vec2f uv = (vec2f(x,y) + sampler->get2f()) * vec2f(1.0/camera->resx, 1.0/camera->resy);
				Ray ray = camera->sampleray(uv);
				Color tres = brightness(ray);
				if (norm(tres)<1e8) res += tres;
			}
			film.setPixel(x, y, res/nspp);
		}
		#pragma omp critical
		fprintf(stderr, "\r%.1f%%", 100.0f*(++line_finished)/camera->resy);
	}
	// end timing
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "  " << elapsed_seconds.count() << "s\n";
	// save files
	for (std::string filename : conf["renderer"]["output_files"]) {
		std::cerr << "INFO: Writing result to " << filename << "\n";
		film.saveFile(filename);
	}
}
