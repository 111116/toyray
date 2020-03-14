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
// int max_bounces;


Color normal(Ray ray) {
	HitInfo hit = acc->hit(ray);
	if (!hit) {
		return Color();
	}
	return Color(0.5,0.5,0.5) + 0.5 * hit.Ns;
}


// sample radiance using ray casting
Color brightness(Ray ray, Sampler& sampler) {
	HitInfo hit = acc->hit(ray);
	Color result;
	if (!hit) {
		for (Light* l: global_lights)
			result += l->radiance(ray);
		// if (globalLightProbe) result += lambda * globalLightProbe->radiance(ray.dir);
		return result;
	}
	if (hit.object->emission) {
		result += hit.object->emission->radiance(ray);
	}
	BSDF* bsdf = hit.object->bsdf;
	if (!bsdf) return result;
	// direct light (local)
	for (Light* l: samplable_lights) {
		vec3f dirToLight;
		Color irr = l->sampleIrradiance(hit.p, dirToLight, sampler);
		// don't do shadow ray test for now
		result += irr * std::abs(dot(hit.Ns, dirToLight)) * bsdf->f(-ray.dir, dirToLight, hit.Ns, hit.Ng);
	}
	// if (!samplable_lights.empty())
	// {
	// 	Object* light = samplable_lights[rand() % samplable_lights.size()];
	// 	float pdf;
	// 	Primitive* shape;
	// 	point lightp = light->sample_point(pdf, shape);
	// 	// check if direct light was blocked
	// 	if (pdf > 1e-8) {
	// 		Ray shadowray = {hit.p, normalized(lightp - hit.p)};
	// 		shadowray.origin += 1e-3 * shadowray.dir;
	// 		pdf /= samplable_lights.size();
	// 		vec3f lightN = shape->Ns(lightp);
	// 		float dw = pow(norm(lightp - hit.p), -2) * fabs(dot(shadowray.dir, lightN));
	// 		result += bsdf->f(-ray.dir, shadowray.dir, Ns, Ng) * fabs(dot(Ns, shadowray.dir)) * light->emission->radiance(shadowray) * (dw / pdf);
	// 	}
	// }
	return result;
}


int nspp = 1; // may be overriden in conf


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
#pragma omp parallel for schedule(dynamic)
	for (auto& p: meshref) {
		// decode conf to json
		std::unordered_map<std::string, Json> t;
		t["file"] = p.first.substr(1);
		t["recompute_normals"] = (p.first[0]=='1');
		p.second = new TriangleMesh(t);
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
	modelpath = directoryOf(argv[1]);
	// load scene conf file
	Json conf;
	fin >> conf;
	loadMaterials(conf);
	loadPrimitives(conf);
	loadSources(conf);
	acc = new BVH(objects);
	nspp = conf["renderer"]["spp"];
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
				Color tres = brightness(ray, *sampler);
				/*if (norm(tres)<1e8)*/ res += tres;
			}
			film.setPixel(x, y, res/nspp);
		}
		#pragma omp critical
		fprintf(stdout, "\r%.1f%%", 100.0f*(++line_finished)/camera->resy);
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
	} catch (const char* s) {
		std::cout << "FATAL: " << s << "\n";
		return 1;
	}
}
