#pragma once

#include <string>

std::string modelpath;

std::string directoryOf(std::string filename) {
	while (!filename.empty() && filename.back()!='\\' && filename.back()!='/')
		filename.pop_back();
	return filename;
}