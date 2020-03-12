#ifndef WRITEBMP_H
#define WRITEBMP_H

#include <cstring>
#include <cstdio>

void writeBMP(const char* filename, char* data, int W, int H)
// data: RGB, top left -> bottom right, no padding
{
	FILE* fout = fopen(filename, "wb");
	char h[54] = "BM";
	h[10] = 54; // header size
	h[14] = 40; // info size
	h[26] = 1;
	h[28] = 24; // bit per pixel
	int size = 54 + (W*3+3)/4*4*H;
	memcpy(h+2, &size, 4);
	memcpy(h+18, &W, 4);
	memcpy(h+22, &H, 4);
	fwrite(h, 54, 1, fout);
	for (int y=H-1; ~y; --y) {
		for (int x=0; x<W; ++x)
			for (int k=2; ~k; --k) // RGB -> BGR
				fwrite(data+y*W*3+x*3+k, 1, 1, fout);
		// pad to 4n bytes per row
		fwrite(h+30, 1, W%4, fout);
	}
	fclose(fout);
}

void writeBMP(const char* filename, float* data, int W, int H, float outputgamma = 2.2)
// data: RGB, top left -> bottom right, no padding
{
	char bytedata[W*H*3];
	for (int i=0; i<W*H*3; ++i)
		bytedata[i] = std::min(255, int(data[i]>1? 1: std::pow(data[i],1.0/outputgamma) *255));
	return writeBMP(filename, bytedata, W, H);
}

#endif
