
#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <chrono>

#include "color.h"
#include "lib/consolelog.hpp"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp"

#include "renderer.hpp"
#include "image.hpp"
#include "cameras/cameracollection.hpp"
#include "lights/lightcollection.hpp"
#include "accelarator/bruteforce.hpp"
#include "accelarator/bvhsah.hpp"


bool opt_preview = false;
bool opt_albedo = false;
bool opt_normal = false;
int opt_spp = 0;
// return scene filename
const char* cmdlineparse(int argc, char* argv[]) {
#ifndef NDEBUG
	console.warn("Running in DEBUG mode. EXTREMELY SLOW!\n");
#endif
	int fileno = 0;
	for (int i=1; i<argc; ++i) {
		if (std::string(argv[i]) == "--quiet")
			console.loglevel = 2;
		else if (std::string(argv[i]) == "--preview")
			opt_preview = true;
		else if (std::string(argv[i]) == "--albedo")
			opt_albedo = true;
		else if (std::string(argv[i]) == "--normal")
			opt_normal = true;
		else if (std::string(argv[i]) == "--spp")
			opt_spp = atoi(argv[++i]);
		else {
			fileno = i;
		}
	}
	if (!fileno) {
		console.info("Usage:", argv[0], "[options] <scene json>");
		console.log ("Options:");
		console.log ("  --quiet          Only show warnings and errors");
		console.log ("  --preview        Render with lower quality");
		console.log ("  --spp <n>        Render with n samples per pixel");
		console.log ("  --albedo         Output albedo buffer to albedo-[filename]");
		console.log ("  --normal         Output normal buffer to normal-[filename]");
		throw "scene not specified.";
	}
	return argv[fileno];
}


std::unordered_map<std::string, BSDF*> bsdf;


void loadObjects(const Json& conf, Renderer& renderer)
{
	if (conf.find("bsdfs") != conf.end())
		for (auto o: conf["bsdfs"]) {
			bsdf[o["name"]] = newMaterial(o, bsdf);
		}
	instantiateGeometry(conf);
	for (auto o: conf["primitives"]) {
		Object* newobj;
		BSDF* sd;
		if (o["bsdf"].type() == Json::value_t::object)
			sd = newMaterial(o["bsdf"], bsdf);
		else {
			if (bsdf.find(std::string(o["bsdf"])) == bsdf.end())
				console.warn("Undefined BSDF", o["bsdf"]);
			sd = bsdf[o["bsdf"]];
		}
		newobj = new Object(o, sd);
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
		const char* filename = cmdlineparse(argc, argv);
		// parse commandline args
		std::ifstream fin(filename);
		if (!fin) throw std::runtime_error(std::string("Failed reading scene file ") + filename);
		modelpath = directoryOf(filename);
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
		renderer.camera = newCamera(conf["camera"]);
		if (opt_preview) {
			renderer.nspp = std::max(1, int(sqrt(renderer.nspp)));
			renderer.camera->resx = std::max(1, renderer.camera->resx/2);
			renderer.camera->resy = std::max(1, renderer.camera->resy/2);
		}
		if (opt_spp) {
			renderer.nspp = opt_spp;
		}

		// start rendering
		console.info("Rendering at", renderer.camera->resx, 'x', renderer.camera->resy, 'x', renderer.nspp, "spp");
		console.time("Rendered");
		auto func = &Renderer::radiance;
		if (opt_albedo) func = &Renderer::albedo;
		if (opt_normal) func = &Renderer::normal;
		Image film = renderer.render(func);
		console.timeEnd("Rendered");
		// save files
		for (std::string filename : getOutputFiles(conf["renderer"])) {
			if (opt_albedo) filename = "albedo-" + filename;
			if (opt_normal) filename = "normal-" + filename;
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
