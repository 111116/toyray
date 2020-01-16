#ifndef ENVMAP_H
#define ENVMAP_H

#include "lib/rgbe/rgbe.h"
#include "color.h"
#include "geometry/geometry.hpp"

class Envmap
{
private:
	int width, height;
	float* data;

public:
	Envmap(const char* filename)
	{
		FILE* fin = fopen(filename, "rb");
		int status = RGBE_ReadHeader(fin, &width, &height, NULL);
		if (status == RGBE_RETURN_FAILURE)
		{
			fclose(fin);
			fprintf(stderr, "Failed to load HDR file\n");
			throw -1;
		}
		if (width != 2*height)
		{
			fclose(fin);
			fprintf(stderr, "HDR image w/h should be 2:1\n");
			throw -1;
		}
		data = new float[width*height*3];
		RGBE_ReadPixels_RLE(fin, data, width, height);
		fclose(fin);
	}

	~Envmap()
	{
		delete[] data;
	}

	// vec3 readdata(float x, float y)
	// {
	// 	int iy = y * height;
	// 	int ix = x * width;
	// 	if (ix == width) --ix;
	// 	if (iy == height) --iy;
	// 	return ((vec3*)data)[(iy*width + ix)];
	// }

	vec3 envlight(vec3 dir)
	{
		static const float INVPI = 1/acosf(-1);
		float y = 0.5 - INVPI * atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));
		float x = 1 - INVPI * atan2(dir.x, dir.z);
		int iy = y * height;
		int ix = x * height;
		if (ix<0 || iy<0 || ix>=width || iy>=height)
		{
			fprintf(stderr, "%f %d %d %d %d\n", x, ix, iy, width, height);
		}
		if (ix == width) --ix;
		if (iy == height) --iy;
		// printf("%d %d\n", iy, ix);
		return ((vec3*)data)[(iy*width + ix)];
	}
};

#endif
