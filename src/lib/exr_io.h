#pragma once
bool SaveEXR(const float* rgb, int width, int height, const char* outfilename);
void LoadEXR_RGBA(float** rgba, int* width, int* height, const char* filename);
void LoadEXR_RGB(float** rgb, int* width, int* height, const char* filename);