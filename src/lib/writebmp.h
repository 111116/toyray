#ifndef WRITEBMP_H
#define WRITEBMP_H

void writeBMP(const char* filename, char* data, int W, int H);
void writeBMP(const char* filename, float* data, int W, int H, float outputgamma = 2.2);

#endif