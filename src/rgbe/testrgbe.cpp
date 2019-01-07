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
	delete[] data;
}

