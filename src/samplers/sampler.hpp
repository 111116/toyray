#pragma once

#include "../math/vecfloat.hpp"

class Sampler
{
public:
	// sample [0,1]
	virtual float get1f() = 0;
	// sample [0,1]x[0,1]
	virtual vec2f get2f() = 0;
	// sample {0,1,...,limit-1}
	virtual unsigned get1u(unsigned limit) = 0;
};
