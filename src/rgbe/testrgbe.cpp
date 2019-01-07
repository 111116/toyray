#include <algorithm>
#include "../writebmp.h"
#include "rgbe.h"

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		puts("arg");
		return 0;
	}
	char* filename = argv[1];
	int w, h;
	FILE* fin = fopen(filename, "rb");
	RGBE_ReadHeader(fin, &w, &h, NULL);
	float* data = new float[w*h*3];
	RGBE_ReadPixels_RLE(fin, data, w, h);
	char* output = new char[w*h*3];
	for (int i=0; i<w*h*3; ++i)
		output[i] = std::min(255.f, 255.0f*data[i]);
	writeBMP("o.bmp", output, w, h);
	for (int i=0; i<w*h*3; ++i)
		output[i] = std::min(255.f, 25.50f*data[i]);
	writeBMP("od.bmp", output, w, h);
	for (int i=0; i<w*h*3; ++i)
		output[i] = std::min(255.f, 2.550f*data[i]);
	writeBMP("odd.bmp", output, w, h);
	for (int i=0; i<w*h*3; ++i)
		output[i] = std::min(255.f, 2550.f*data[i]);
	writeBMP("oi.bmp", output, w, h);
	delete[] data;
}

