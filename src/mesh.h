#pragma once

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include "geometry.h"


struct Mesh {
	std::vector<Triangle*> faces;
	// load mesh from an ascii Wavefront .obj file
	// only supports triangle mesh with 2d texture
	void loadFromFile(const char* filename) {
		if (strcmp(filename+strlen(filename)-4, ".obj") != 0)
			throw "format must be obj";
		std::ifstream fin(filename);
		if (!fin)
			throw "Can't open file";

		std::vector<vec3> v,vn;
		std::vector<vec2> vt;
		std::string line;
		// reading file line by line
		while (std::getline(fin, line)) {
			if (isalpha(line[0])) {
				std::stringstream in(line);
				std::string cmd;
				in >> cmd;
				if (cmd == "v") { // vertex coordinate
					vec3 t;
					in >> t;
					v.push_back(t);
				}
				if (cmd == "vt") { // texture coordinate
					vec2 t;
					in >> t;
					vt.push_back(t);
				}
				if (cmd == "vn") { // normal vector
					vec3 t;
					in >> t;
					vn.push_back(t);
				}
				if (cmd == "f") { // polygon face
					std::vector<vec3> vv, vvn;
					std::vector<vec2> vvt;
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
				        if (!indices[0])
				        	throw "error parsing obj";
				        int iv = indices[0];
				        int ivt = indices[1];
				        int ivn = indices[2];
				        if (iv < 0) iv += v.size()+1;
				        if (ivt < 0) ivt += vt.size()+1;
				        if (ivn < 0) ivn += vn.size()+1;
				    	vv.push_back(v[iv-1]);
				    	// std::cout << "vv " << iv << " " << v[iv-1] << std::endl;
				    	vvt.push_back(ivt? vt[ivt-1]: vec2());
				    	vvn.push_back(ivn? vn[ivn-1]: vec3(0,1,0));
				    }
				    if (vv.size() == 3) {
				    	faces.push_back(new Triangle(vv[0], vv[1], vv[2], vvt[0], vvt[1], vvt[2], vvn[0], vvn[1], vvn[2]));
				    }
				    else {
				    	throw "unexpected number of vertices";
				    }
				} // end reading face
			}
		} // end reading obj file
	}

	
};