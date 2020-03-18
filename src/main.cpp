// ray casting

#include <assert.h>

#ifdef THREADED
#include <omp.h>
#endif

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>

#include "color.h"
#include "lib/saveexr.h"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp" // model directory
#include "lib/consolelog.hpp"

#include "object.hpp"
#include "lights/lighttypes.hpp"
#include "accelarator/bvhsah.hpp"
#include "accelarator/bruteforce.hpp"
#include "samplers/randomsampler.hpp"
#include "film.hpp"
#include "cameras/camera.hpp"



std::vector<Object*> objects;
std::vector<Light*> samplable_lights;
std::vector<Light*> global_lights;
Accelarator* acc;
std::unordered_map<std::string, BSDF*> bsdf;
int max_bounces = 16;
// TODO don't use fixed eps
const float geoEPS = 1e-3;


Color normal(Ray ray, Sampler&) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		return Color();
	}
	return Color(0.5,0.5,0.5) + 0.5 * hit.Ns;
}


// sample radiance using ray casting
Color radiance(Ray ray, Sampler& sampler)
{
	Color through(1);
	Color result;
	bool lastDirac = true; // accept all light sources if directly into camera
	for (int nbounce = 0;; ++nbounce)
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
		if (nbounce == max_bounces) break;
		// direct light (local) // TODO
		for (Light* l: samplable_lights) {
			vec3f dirToLight;
			float dist;
			Color irr = l->sampleIrradiance(hit.p, dirToLight, dist, sampler);
			dist -= geoEPS; // cancel amount that ray origin is moved forward
			// shadow ray test
			Ray shadowray(hit.p + geoEPS * dirToLight, dirToLight);
			HitInfo shadowhit = acc->hit(shadowray);
			if (!shadowhit || norm(shadowhit.p - shadowray.origin) > (1-geoEPS) * dist)
				result += through * irr * fabs(dot(hit.Ns, dirToLight)) * bsdf->f(-ray.dir, dirToLight, hit.Ns, hit.Ng);
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



void welcome(int argc, char* argv[]) {
#ifndef NDEBUG
	console.warn("Running in DEBUG mode. EXTREMELY SLOW!\n");
#endif
#ifdef THREADED
#pragma omp parallel
	{
	#pragma omp single
		console.info(omp_get_num_threads(), "THREADS");
	}
#else
	console.info("Threading disabled");
#endif
	if (argc<=1) {
		console.info("Usage:", argv[0], "<json file>");
		throw "scene not specified.";
	}
}


void loadMaterials(const Json& conf) {
	for (auto o: conf["bsdfs"]) {
		bsdf[o["name"]] = newMaterial(o);
	}
}
void loadPrimitives(const Json& conf)
{
	instantiateGeometry(conf);
	for (auto o: conf["primitives"]) {
		// if (o["type"] == "infinite_sphere") {
		// 	// assume infinite_sphere will only be used for light probes
		// 	globalLightProbe = new LightProbe(getpath(o["emission"]).c_str());
		// 	// TODO currently light probes are not samplable
		// }
		// else {
			Object* newobj;
			if (bsdf.find(std::string(o["bsdf"])) == bsdf.end())
				console.warn("Undefined BSDF", o["bsdf"]);
			newobj = new Object(o, bsdf[o["bsdf"]]);
			objects.push_back(newobj);
		// }
	}
}

void loadSources(const Json& conf) {
	// add light sources
	for (auto o: objects) {
		if (o->emission && o->emission->samplable)
			samplable_lights.push_back(o->emission);
	}
	if (conf.find("emissions") != conf.end()) {
		for (auto o: conf["emissions"]) {
			Light* light = newLight(o);
			if (light->samplable) {
				samplable_lights.push_back(light);
			}
			else {
				global_lights.push_back(light);
			}
		}
	}
}

std::vector<std::string> getOutputFiles(const Json& conf)
{
	std::vector<std::string> v;
	if (conf.find("output_files") != conf.end())
		for (std::string s: conf["output_files"])
			v.push_back(s);
	if (conf.find("output_file") != conf.end())
		v.push_back(conf["output_file"]);
	if (conf.find("hdr_output_file") != conf.end())
		v.push_back(conf["hdr_output_file"]);
	return v;
}


int main(int argc, char* argv[])
{
	try {
		welcome(argc, argv);
		// parse commandline args
		std::ifstream fin(argv[1]);
		if (!fin) throw "Failed reading scene file";
		modelpath = directoryOf(argv[1]);
		// load scene conf file
		Json conf;
		fin >> conf;
		loadMaterials(conf);
		loadPrimitives(conf);
		loadSources(conf);
		acc = new BVH(objects);
		if (conf.find("integrator") != conf.end()) {
			if (conf["integrator"].find("max_bounces") != conf["integrator"].end())
				max_bounces = conf["integrator"]["max_bounces"];
		}
		int nspp = conf["renderer"]["spp"];
		Camera* camera = newCamera(conf["camera"]);

		Film film(camera->resx, camera->resy);
		console.info("Rendering at", camera->resx, 'x', camera->resy, 'x', nspp, "spp");
		// start rendering
		console.time("Rendered");
		int line_finished = 0;
#pragma omp parallel for schedule(dynamic)
		for (int y=0; y<camera->resy; ++y) {
			for (int x=0; x<camera->resx; ++x) {
				Color res;
				for (int i=0; i<nspp; ++i) {
					Sampler* sampler = new RandomSampler();
					vec2f uv = (vec2f(x,y) + sampler->get2f()) * vec2f(1.0/camera->resx, 1.0/camera->resy);
					Ray ray = camera->sampleray(uv);
					Color tres = radiance(ray, *sampler);
					/*if (norm(tres)<1e8)*/ res += tres;
				}
				film.setPixel(x, y, res/nspp);
			}
#pragma omp critical
			// report progress
			fprintf(stderr, "\r%.1f%%", 100.0f*(++line_finished)/camera->resy);
		}
		console.timeEnd("Rendered");
		// save files
		for (std::string filename : getOutputFiles(conf["renderer"])) {
			console.info("Writing result to", filename);
			film.saveFile(filename);
		}
	}
	catch (const char* s)
	{
		console.error("FATAL:", s);
		return 1;
	}
	catch (std::runtime_error err)
	{
		console.error("FATAL:", err.what());
		return 1;
	}
}
