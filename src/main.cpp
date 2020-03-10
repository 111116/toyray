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
	// 		Ray shadowray = {hit.p, normalize(lightp - hit.p)};
	// 		shadowray.origin += 1e-3 * shadowray.dir;
	// 		pdf /= samplable_light_objects.size();
	// 		vec3 lightN = shape->Ns(lightp);
	// 		float dw = pow(norm(lightp - hit.p), -2) * fabs(dot(shadowray.dir, lightN));
	// 		result += bsdf->f(-ray.dir, shadowray.dir, Ns, Ng) * fabs(dot(Ns, shadowray.dir)) * light->emission->radiance(shadowray) * (dw / pdf);
	// 	}
	// }
	// // patch for Phong ambient
	// if (bsdf->component)
	// {
	// 	Phong* phong = dynamic_cast<Phong*>(bsdf->component);
	// 	if (phong != NULL) {
	// 		result += phong->Ka;
	// 	}
	// }
	return result;
}


float* pixels;
int nspp = 4; // default number of samples per pixel, may be overrided in parameters
std::string outfilename;


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
