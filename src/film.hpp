#pragma once

#include "color.h"

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
};