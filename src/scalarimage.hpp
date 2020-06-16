#pragma once

#include <iostream>
#include "color.h"
#include "lib/exr_io.h"
#include "lib/stb_image.h"
#include "lib/writebmp.h"
#include "lib/consolelog.hpp"

class ScalarImage
{
	float* pixels;

public:
	int w,h;

	ScalarImage(const std::string& filename)
	{
		std::string ext = suffix(filename);
		console.log("Loading ScalarImage:", filename);
		if (ext == "hdr" || ext == "png" || ext == "jpg") {
			// use the last channel
			int n;
			float* raw = stbi_loadf(filename.c_str(), &w, &h, &n, 0);
			if (raw == NULL) throw "ScalarImage load failed";
			pixels = new float[w*h];
			for (int i=0; i<w*h; ++i)
				pixels[i] = raw[i*n+n-1];
			stbi_image_free(raw);
			return;
		}
		throw "ScalarImage load: unrecognized format";
	}

	float getPixel(int x, int y) {
		return pixels[y*w+x];
	}

	// nearest neighbor sample; position is modulated to [0,1]x[0,1]
	float sample(const vec2f& uv) {
		return getPixel( (int(uv.x*w)%w+w)%w, (int(uv.y*h)%h+h)%h );
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