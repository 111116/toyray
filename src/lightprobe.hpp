#pragma once

#include "lib/rgbe/rgbe.h"
#include "color.h"
#include "geometry/geometry.hpp"
#include "jsonutil.hpp"

class LightProbe
{
private:
	int width, height;
	float* data;

public:
	LightProbe(const char* filename)
	{
		std::cout << filename << std::endl;
		FILE* fin = fopen(filename, "rb");
		assert(fin!=NULL);
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

	~LightProbe()
	{
		delete[] data;
	}

	// dir: outward unit vector
	vec3 radiance(vec3 dir)
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
		return ((vec3*)data)[(iy*width + ix)];
	}
};

