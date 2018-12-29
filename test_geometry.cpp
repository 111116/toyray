#include <cstdio>
#include <iostream>
#include "geometry.h"

std::ostream& operator << (std::ostream& os, vec3 v)
{
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}

int main()
{
	triangle t;
	t.v1 = vec3(0,0,0);
	t.v2 = vec3(0,1,0);
	t.v3 = vec3(0,0,1);
	t.vn1 = vec3(1,0,0);
	t.vn2 = vec3(1,0,0);
	t.vn3 = vec3(1,0,0);
	t.preprocess();
	
	Ray r = {{2,3,3}, {0,1,0}};
	point p = t.intersection(r);
	std::cout << p << std::endl;
}
