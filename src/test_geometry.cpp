#include <cstdio>
#include <iostream>
#include "geometry.h"

vec3 sampleInUnitSphere()
{
	vec3 v;
	do v = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
	while (norm(v)>1);
	return v;
}

vec3 sampleOverHemisphericalSurface()
{
	vec3 v;
	do v=sampleInUnitSphere();
	while (norm(v) < 1e-2);
	if (v.y<0) v.y = -v.y;
	return normalize(v);
}

int main()
{
	double s = 0;
	int n = 1000000;
	for (int i=0; i<n; ++i)
	{
		vec3 v = sampleOverHemisphericalSurface();
		s += dot(vec3(0,1,0), v);
	}
	std::cout << s/n << std::endl;
	// result should be hemispherical integral / hemispherical surface area
	// = integral / (2pi)
	// = 0.5
}
