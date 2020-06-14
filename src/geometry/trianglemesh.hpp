#pragma once

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

#include "util/jsonutil.hpp"
#include "util/filepath.hpp"
#include "lib/consolelog.hpp"

#include "triangle.hpp"
#include "container.hpp"



class TriangleMesh: public BasicContainer
{
public:
	TriangleMesh(std::string filename):
		BasicContainer(loadfromfile(getpath(filename).c_str())) {}

private:
	// load mesh from an ascii Wavefront .obj file
	// only supports triangle mesh with 2d texture
	std::vector<BasicPrimitive*> loadfromfile(const char* filename)
	{
		console.log("Loading mesh:", filename);
		if (strcmp(filename+strlen(filename)-4, ".obj") != 0)
			throw "format must be obj";
		std::ifstream fin(filename);
		if (!fin)
			throw "Can't open file";
		std::vector<BasicPrimitive*> faces;
		std::vector<vec3f> v,vn;
		std::vector<vec2f> vt;
		std::string line;
		// reading file line by line
		while (std::getline(fin, line)) {
			if (isalpha(line[0])) {
				std::stringstream in(line);
				std::string cmd;
				in >> cmd;
				if (cmd == "v") { // vertex coordinate
					vec3f t;
					in >> t;
					v.push_back(t);
				}
				if (cmd == "vt") { // texture coordinate
					vec2f t;
					in >> t;
					vt.push_back(t);
				}
				if (cmd == "vn") { // normal vector
					vec3f t;
					in >> t;
					vn.push_back(t);
				}
				if (cmd == "f") { // polygon face
					std::vector<vec3f> vv, vvn;
					std::vector<vec2f> vvt;
					bool recompute_normal = false;
					while (!in.fail() && !in.eof()) {					
						// code from tungsten ObjLoader::loadFace
						int indices[] = {0, 0, 0};
						for (int i = 0; i < 3; ++i) {
							if (in.peek() != '/')
								in >> indices[i];
							if (in.peek() == '/')
								in.get();
							else
								break;
						}
						while (isspace(in.peek()))
							in.get();
						if (!indices[0]) {
							console.log("Obj line:", line);
							throw "error parsing obj";
						}
						int iv = indices[0];
						int ivt = indices[1];
						int ivn = indices[2];
						if (iv < 0) iv += v.size()+1;
						if (ivt < 0) ivt += vt.size()+1;
						if (ivn < 0) ivn += vn.size()+1;
						vv.push_back(v[iv-1]);
						vvt.push_back(ivt? vt[ivt-1]: vec2f());
						vvn.push_back(ivn? vn[ivn-1]: vec3f(0,1,0));
						recompute_normal |= !ivn;
					}
					if (vv.size() == 3) {
						// ignore triangles of zero surface area
						if (cross(vv[1]-vv[0],vv[2]-vv[0]) != vec3f(0)) {
							faces.push_back(new Triangle(vv[0], vv[1], vv[2], vvt[0], vvt[1], vvt[2], vvn[0], vvn[1], vvn[2]));
							if (recompute_normal)
								dynamic_cast<Triangle*>(faces.back())->recompute_normal();
						}
					}
					else {
						throw "mesh face other than triangle not supported";
					}
				} // end reading face
			}
		}
		return faces;
	}
};
