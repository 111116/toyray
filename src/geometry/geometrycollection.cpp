
#include "geometry.hpp"
#include "util/jsonutil.hpp"
#include "util/taskscheduler.hpp"

#include <unordered_map>
#include "triangle.hpp"
#include "trianglemesh.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "quad.hpp"
#include "cube.hpp"

void instantiateGeometry(const Json& conf);
Primitive* newPrimitive(const Json& conf);



namespace
{
	std::string encodemesh(const Json& o)
	{
		bool recompute_normals = true;
		if (o.find("recompute_normals") != o.end()) {
			recompute_normals = o["recompute_normals"];
		}
		return (recompute_normals?"1":"0") + std::string(o["file"]);
	};
	Json decodemesh(const std::string& s)
	{
		std::unordered_map<std::string, Json> t;
		t["file"] = s.substr(1);
		t["recompute_normals"] = (s[0]=='1');
		return t;
	}
	std::unordered_map<std::string, Primitive*> meshref;
}


void instantiateGeometry(const Json& conf)
{
	// auto instancing for meshes
	for (auto o: conf["primitives"])
	{
		if (o["type"] == "mesh") {
			meshref[encodemesh(o)] = NULL;
		}
	}
	// flatten map elements for parallelization
	std::vector<std::unordered_map<std::string, Primitive*>::iterator> v;
	for (auto it = meshref.begin(); it != meshref.end(); ++it)
		v.push_back(it);

	// start instantiation
	TaskScheduler tasks;
	for (auto p = v.begin(); p != v.end(); ++p)
		tasks.add([&](){
			(*p)->second = new TriangleMesh(decodemesh((*p)->first));
		});
	tasks.start();
}


Primitive* newPrimitive(const Json& conf)
{
	Primitive* shape = NULL;
	if (conf["type"] == "mesh") {
		shape = meshref[encodemesh(conf)];
	}
	if (conf["type"] == "sphere") {
		shape = new Sphere(json2vec3f(conf["origin"]), (double)conf["radius"]);
	}
	if (conf["type"] == "plane") {
		shape = new Plane(json2vec3f(conf["normal"]), (double)conf["offset"]);
	}
	if (conf["type"] == "cube") {
		shape = new Cube();
	}
	if (conf["type"] == "quad") {
		shape = new Quad();
	}
	if (conf["type"] == "triangle") {
		vec3f v1,v2,v3,n1,n2,n3;
		bool recompute_normal = true;
		v1 = json2vec3f(conf["vertices"][0]);
		v2 = json2vec3f(conf["vertices"][1]);
		v3 = json2vec3f(conf["vertices"][2]);
		if (conf.find("normals") != conf.end()) {
			recompute_normal = false;
			n1 = json2vec3f(conf["normals"][0]);
			n2 = json2vec3f(conf["normals"][1]);
			n3 = json2vec3f(conf["normals"][2]);
		}
		vec2f t1,t2,t3;
		if (conf.find("uvs") != conf.end()) {
			t1 = json2vec2f(conf["uvs"][0]);
			t2 = json2vec2f(conf["uvs"][1]);
			t3 = json2vec2f(conf["uvs"][2]);
		}
		Triangle* tshape = new Triangle(v1,v2,v3,t1,t2,t3,n1,n2,n3);
		if (conf.find("recompute_normals") != conf.end())
			recompute_normal = conf["recompute_normals"];
		if (recompute_normal)
			tshape->recompute_normal();
		shape = tshape;
	}
	if (shape == NULL) {
		throw std::runtime_error("Unrecognized Geometric type: " + std::string(conf["type"]));
	}
	return shape;
}