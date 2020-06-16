#pragma once

#include <iostream>
#include "color.h"
#include "lib/exr_io.h"
#include "lib/stb_image.h"
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
		console.log("Loading image:", filename);
		if (ext == "exr") {
			LoadEXR_RGB(&pixels, &w, &h, filename.c_str());
			return;
		}
		if (ext == "hdr" || ext == "png" || ext == "jpg") {
			int n;
			pixels = stbi_loadf(filename.c_str(), &w, &h, &n, 3);
			return;
		}
		throw "Image load: unrecognized format";
	}

	Color getPixel(int x, int y) {
		return Color(pixels[3*(y*w+x)], pixels[3*(y*w+x)+1], pixels[3*(y*w+x)+2]);
	}

	// nearest neighbor sample; position is modulated to [0,1]x[0,1]
	Color sample(const vec2f& uv) {
		return getPixel( (int(uv.x*w)%w+w)%w, (int(uv.y*h)%h+h)%h );
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