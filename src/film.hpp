#pragma once

#include <iostream>
#include "color.h"
#include "lib/saveexr.h"
#include "lib/writebmp.h"
#include "lib/consolelog.hpp"

class Film
{
	float* pixels;
public:
	const int w,h;
	Film(int w, int h): w(w), h(h) {
		pixels = new float[w*h*3];
	}
	void setPixel(int x, int y, const Color& val) {
		int pxid = y*w + x;
		pixels[3*pxid+0] = val.x;
		pixels[3*pxid+1] = val.y;
		pixels[3*pxid+2] = val.z;
	}
	// supported suffix: bmp, exr
	int saveFile(const std::string& filename) const
	{
		int dotpos = filename.rfind(".");
		if (dotpos == std::string::npos) {
			console.warn("saveFile: unrecognized format");
			return -1;
		}
		auto suffix = filename.substr(dotpos+1);

		if (suffix == "exr") {
			SaveEXR(pixels, w, h, filename.c_str());
			return 0;
		}
		if (suffix == "bmp") {
			writeBMP(filename.c_str(), pixels, w, h);
			return 0;
		}
		console.warn("saveFile: unrecognized format", suffix);
		return -1;
	}
};