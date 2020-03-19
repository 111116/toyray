#pragma once
bool SaveEXR(const float* rgb, int width, int height, const char* outfilename);
void LoadEXR(float** rgba, int* width, int* height, const char* filename);