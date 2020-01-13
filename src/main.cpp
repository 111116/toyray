// naive path tracing implementation


#include <algorithm>
#include <vector>
#include <fstream>
#include "geometry.h"
#include "envmap.h"
// #include "writebmp.h"
#include "saveexr.h"
#include "jsonutil.hpp"
#include "env.hpp" // model directory
#include "object.hpp"
#include "camera.h"
#include "bruteforce.hpp"

#define DEBUG


float* pixels;
int nspp = 4; // default number of samples per pixel, may be overrided in parameters

std::vector<Object*> objects;
Bruteforce* acc;
std::unordered_map<std::string, BsDF*> bsdf;
std::string outfilename;
int max_bounces;

vec3 cast_albedo(Ray ray, int depth) {
	std::pair<Primitive*, Object*> hit = acc->hit(ray);
	if (hit.first == NULL) {
		return vec3();
	}
	// return hit.second->bsdf->albedo;
	point p;
	hit.first->intersect(ray, &p);
	return vec3(0.5,0.5,0.5)+0.5*hit.first->Ns(p);
}
vec3 cast(Ray ray, int depth) {
	return cast_albedo(ray, 0);
}

int main(int argc, char* argv[])
{
#ifdef DEBUG
	fprintf(stderr, "WARNING: running in debug mode.\n");
#endif
	if (argc<=1) {
		std::cerr << "Usage: " << argv[0] << " conf.json"<< std::endl;
		return 1;
	}
	std::ifstream fin(argv[1]);
	modelpath = directoryOf(argv[1]);
	// load scene conf file
	Json conf;
	fin >> conf;
	for (auto o: conf["bsdfs"]) {
		bsdf[o["name"]] = new BsDF(o);
	}
	for (auto o: conf["primitives"]) {
		objects.push_back(new Object(o, bsdf[o["bsdf"]]));
	}
	acc = new Bruteforce(objects);
	Camera* camera = new Camera(conf["camera"]);
	assert(conf["integrator"]["type"] == "path_tracer");
	assert(conf["integrator"]["enable_light_sampling"] == true);
	max_bounces = conf["integrator"]["max_bounces"];
	nspp = conf["renderer"]["spp"];
	outfilename = conf["renderer"]["hdr_output_file"];

	// prepare film
	pixels = new float[camera->resolution_x * camera->resolution_y * 3];
	fprintf(stderr, "INFO: rendering at %d x %d x %d spp\n", camera->resolution_x, camera->resolution_y, nspp);
	// start rendering
	for (int y=0, byteCnt=0; y<camera->resolution_y; ++y)
	{
		for (int x=0; x<camera->resolution_x; ++x)
		{
			vec3 res;
			for (int i=0; i<nspp; ++i)
			{
				// manual camera setup
				float w = 4;
				float u = (x+randf()) / camera->resolution_x;
				float v = (y+randf()) / camera->resolution_y;
				Ray ray = camera->sampleray(u,v);
				res += cast(ray, 0);
			}
			res *= 1.0/nspp;
			pixels[byteCnt++] = res.x;
			pixels[byteCnt++] = res.y;
			pixels[byteCnt++] = res.z;
		}
		fprintf(stderr, "\r%.1f%%", 100.0f*(y+1)/camera->resolution_y);
	}
	fprintf(stderr, "INFO: Writing result to %s\n", outfilename.c_str());
	SaveEXR(pixels, camera->resolution_x, camera->resolution_y, outfilename.c_str());
}