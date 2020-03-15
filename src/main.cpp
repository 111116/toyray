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
#include "jsonutil.hpp"
#include "filepath.hpp" // model directory

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


Color normal(Ray ray) {
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
			// shadow ray test // TODO auto error instead of fixed 1e-3, 0.999
			Ray shadowray(hit.p + 1e-3 * dirToLight, dirToLight);
			HitInfo shadowhit = acc->hit(shadowray);
			if (!shadowhit || norm(shadowhit.p - shadowray.origin) > 0.999 * dist)
				result += through * irr * fabs(dot(hit.Ns, dirToLight)) * bsdf->f(-ray.dir, dirToLight, hit.Ns, hit.Ng);
		}
		// indirect light (bsdf importance sampling)
		vec3f newdir;
		Color f = bsdf->sample_f(-ray.dir, newdir, hit.Ns, hit.Ng, lastDirac, sampler); // already scaled by 1/pdf
		through *= fabs(dot(newdir, hit.Ns)) * f;
		if (through == vec3f() || !(sqrlen(through) < 1e20)) break;
		// TODO auto error instead of fixed 1e-3
		ray = Ray(hit.p + 1e-3*newdir, newdir);
	}
	return result;
}



void welcome(int argc, char* argv[]) {
#ifndef NDEBUG
	fprintf(stderr, "WARNING: running in DEBUG mode. EXTREMELY SLOW!\n");
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
	}
}


void loadMaterials(const Json& conf) {
	for (auto o: conf["bsdfs"]) {
		bsdf[o["name"]] = newMaterial(o);
	}
}
void loadPrimitives(const Json& conf) {
	// auto instancing for meshes
	auto encode = [](const Json& o) {
		bool recompute_normals = true;
		if (o.find("recompute_normals") != o.end()) {
			recompute_normals = o["recompute_normals"];
		}
		return (recompute_normals?"1":"0") + std::string(o["file"]);
	};
	std::unordered_map<std::string, Primitive*> meshref;
	for (auto o: conf["primitives"]) {
		if (o["type"] == "mesh") {
			meshref[encode(o)] = NULL;
		}
	}
	// flatten map elements for parallelization
	std::vector<std::unordered_map<std::string, Primitive*>::iterator> v;
	for (auto it = meshref.begin(); it != meshref.end(); ++it)
		v.push_back(it);
#pragma omp parallel for schedule(dynamic)
	for (auto p = v.begin(); p != v.end(); ++p) {
		// decode conf to json
		std::unordered_map<std::string, Json> t;
		t["file"] = (*p)->first.substr(1);
		t["recompute_normals"] = ((*p)->first[0]=='1');
		(*p)->second = new TriangleMesh(t);
	}
	for (auto o: conf["primitives"]) {
		// if (o["type"] == "infinite_sphere") {
		// 	// assume infinite_sphere will only be used for light probes
		// 	globalLightProbe = new LightProbe(getpath(o["emission"]).c_str());
		// 	// TODO currently light probes are not samplable
		// }
		// else {
			Object* newobj;
			Primitive* instancing = (o["type"] == "mesh")? meshref[encode(o)]: NULL;
			if (bsdf.find(std::string(o["bsdf"])) == bsdf.end())
				std::cerr << "WARNING: UNDEFINED BSDF " + std::string(o["bsdf"]) << std::endl;
			newobj = new Object(o, bsdf[o["bsdf"]], instancing);
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


int main(int argc, char* argv[])
{
	try {
	welcome(argc, argv);
	// parse commandline args
	if (argc<=1) return 1;
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
	int nspp = conf["renderer"]["spp"];
	Camera* camera = newCamera(conf["camera"]);
	Film film(camera->resx, camera->resy);
	fprintf(stdout, "Rendering at %d x %d x %d spp\n", camera->resx, camera->resy, nspp);
	// start rendering
	auto start = std::chrono::system_clock::now();
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
		fprintf(stderr, "\r%.1f%%", 100.0f*(++line_finished)/camera->resy);
	}
	// end timing
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "  " << elapsed_seconds.count() << "s\n";
	// save files
	for (std::string filename : conf["renderer"]["output_files"]) {
		std::cout << "Writing result to " << filename << "\n";
		film.saveFile(filename);
	}
	}
	catch (const char* s)
	{
		std::cout << "FATAL: " << s << "\n";
		return 1;
	}
	catch (std::runtime_error err)
	{
		std::cerr << "FATAL: " << err.what() << std::endl;
		return 1;
	}
}
