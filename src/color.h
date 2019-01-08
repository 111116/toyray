#ifndef COLOR_H
#define COLOR_H

#define RGB_COLOR
// #define SPECTRUM_COLOR

#ifdef RGB_COLOR
	#include "la.h"
	typedef vec3 Color;
#else
	typedef float Color;
#endif


#endif
