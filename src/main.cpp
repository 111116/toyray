
#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <chrono>

#include "color.h"
#include "lib/saveexr.h"
#include "lib/consolelog.hpp"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp"
#include "util/taskscheduler.hpp"

#include "renderer.hpp"
#include "film.hpp"
#include "cameras/cameracollection.hpp"
#include "lights/lightcollection.hpp"
#include "accelarator/bruteforce.hpp"
#include "accelarator/bvhsah.hpp"



void welcome(int argc, char* argv[]) {
#ifndef NDEBUG
	console.warn("Running in DEBUG mode. EXTREMELY SLOW!\n");
#endif
	if (argc<=1) {
		console.info("Usage:", argv[0], "<json file>");
		throw "scene not specified.";
	}
}


std::unordered_map<std::string, BSDF*> bsdf;


void loadObjects(const Json& conf, Renderer& renderer)
{
	for (auto o: conf["bsdfs"]) {
		bsdf[o["name"]] = newMaterial(o);
	}
	instantiateGeometry(conf);
	for (auto o: conf["primitives"]) {
		Object* newobj;
		if (bsdf.find(std::string(o["bsdf"])) == bsdf.end())
			console.warn("Undefined BSDF", o["bsdf"]);
		newobj = new Object(o, bsdf[o["bsdf"]]);
		renderer.objects.push_back(newobj);
	}
}

void loadSources(const Json& conf, Renderer& renderer) {
	// add light sources
	for (auto o: renderer.objects) {
		if (o->emission && o->emission->samplable)
			renderer.samplable_lights.push_back(o->emission);
	}
	if (conf.find("emissions") != conf.end()) {
		for (auto o: conf["emissions"]) {
			Light* light = newLight(o);
			if (light->samplable) {
				renderer.samplable_lights.push_back(light);
			}
			else {
				renderer.global_lights.push_back(light);
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
		Renderer renderer;
		loadObjects(conf, renderer);
		loadSources(conf, renderer);
		renderer.acc = new BVH(renderer.objects);
		if (conf.find("integrator") != conf.end()) {
			if (conf["integrator"].find("max_bounces") != conf["integrator"].end())
				renderer.max_bounces = conf["integrator"]["max_bounces"];
		}
		renderer.nspp = conf["renderer"]["spp"];
		Camera* camera = newCamera(conf["camera"]);
		renderer.camera = camera;

		Film film(camera->resx, camera->resy);
		console.info("Rendering at", camera->resx, 'x', camera->resy, 'x', renderer.nspp, "spp");
		// start rendering
		console.time("Rendered");
		TaskScheduler tasks;
		for (int y=0; y<camera->resy; ++y) {
			tasks.add([&,y](){
				for (int x=0; x<camera->resx; ++x) {
					vec2f pixelsize = vec2f(1.0/camera->resx, 1.0/camera->resy);
					vec2f pixelpos = vec2f(x,y) * pixelsize;
					film.setPixel(x, y, renderer.render(pixelpos, pixelsize));
				}
			});
		}
		tasks.onprogress = [](int k, int n){
			fprintf(stderr, "\r    %.1f%% ", 100.0f*k/n);
		};
		tasks.start();
		console.log(); // new line after progress
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
