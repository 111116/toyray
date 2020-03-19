#pragma once

#include <iostream>
#include "color.h"
#include "lib/exr_io.h"
#include "lib/writebmp.h"
#include "lib/consolelog.hpp"

class Image
{
	float* pixels;

public:
	int w,h;

	Image(int w, int h): w(w), h(h) {
		pixels = new float[w*h*3];
	}

	Image(const std::string& filename) {
		std::string ext = suffix(filename);
		if (ext == "exr") {
			LoadEXR_RGB(&pixels, &w, &h, filename.c_str());
			return;
		}
		throw "Image load: unrecognized format";
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
		std::string ext = suffix(filename);
		if (ext == "") {
			console.warn("saveFile: unrecognized format");
			return -1;
		}
		if (ext == "exr") {
			SaveEXR(pixels, w, h, filename.c_str());
			return 0;
		}
		if (ext == "bmp") {
			writeBMP(filename.c_str(), pixels, w, h);
			return 0;
		}
		console.warn("saveFile: unrecognized format", ext);
		return -1;
	}

private:
	static std::string suffix(const std::string& filename)
	{
		int dotpos = filename.rfind(".");
		if (dotpos == std::string::npos) {
			return "";
		}
		return filename.substr(dotpos+1);
	}
};