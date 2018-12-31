#include <algorithm>
#include <vector>
#include <fstream>
#include "writebmp.h"
#include "geometry.h"

#define DEBUG


float randf()
{
	return (float)rand()/RAND_MAX;
}




struct face
{
	enum attrtype
	{
		LIGHT, DIFFUSE
	};
	attrtype attr;
	Primitive* shape;
	vec3 color;
};

std::vector<face> objects;



void loadShapes()
{
	std::ifstream fin("mesh");
	std::string type, strattr;
	while (fin >> type >> strattr)
	{
		Primitive* shape;
		if (type == "triangle")
		{
			Triangle* t = new(Triangle);
			fin >> t->v1 >> t->v2 >> t->v3 >> t->vn1;
			t->vn2 = t->vn3 = t->vn1;
			t->preprocess();
			shape = t;
		}
		if (type == "sphere")
		{
			vec3 origin;
			float radius;
			std::cin >> origin >> radius;
			shape = new(Sphere)(origin, radius);
		}
		face::attrtype attr;
		if (strattr == "light") attr = face::LIGHT;
		if (strattr == "diffuse") attr = face::DIFFUSE;
		vec3 color;
		fin >> color;
		objects.push_back((face){attr, shape, color});
	}
}



void loadSAS()
{
	// testing brute force. no acceleration structure
}



vec3 outDiffuse(vec3 Normal)
{
	vec3 res;
	do res = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
	while (norm(res) > 1);
	return normalize(1.00001 * Normal + res);
}


long long nRay = 0;


face* hitAnything(Ray ray)
{
	face* hit = NULL;
	float dist;
	point res;
	// bruteforcing checking against every primitive
	for (face& shape: objects)
		if (shape.shape->intersect(ray, &res))
			if (hit == NULL || dist > norm(res - ray.origin))
				hit = &shape,
				dist = norm(res - ray.origin);
	return hit;
}


vec3 cast(Ray ray)
{
	// prevent ray intersecting its source surface
	ray.origin += 1e-5 * ray.dir;
	// count number of ray casted
	nRay++;

	// printf("%f %f %f %f %f %f\n",ray.origin.x,ray.origin.y,ray.origin.z,ray.dir.x,ray.dir.y,ray.dir.z);

#ifdef DEBUG
	assert(abs(norm(ray.dir) - 1) < 1e-5);
#endif

	face* hit = hitAnything(ray);
	if (hit == NULL)
		return vec3(0,0,0);
	if (hit->attr == face::LIGHT)
		return hit->color;
	point p;
	assert(hit->shape->intersect(ray, &p));
	vec3 color = hit->color;
	float prob = std::max(color.x, std::max(color.y, color.z));
	if (randf() < prob) 
	{
		vec3 inColor = cast((Ray){p, outDiffuse(hit->shape->normalAtPoint(p))});
		inColor.x *= color.x;
		inColor.y *= color.y;
		inColor.z *= color.z;
		return 1/prob * inColor;
	}
	return vec3(0,0,0);

	// reflection
		// point p = origin.intersection(ray);
		// vec3 N = origin.normalAtPoint(p);
		// vec3 newdir = ray.dir - 2 * N * dot(N, ray.dir);
		// return cast((Ray){p, newdir});

	// refraction
		// float kk = 1.6;
		// if (dot(N, ray.dir) < 0)
		// {
		// 	float theta = acos(dot(-N, ray.dir));
		// 	newdir = normalize(tan(asin(sin(theta)/kk)) * normalize(ray.dir - N * dot(N, ray.dir)) + normalize(-N));
		// }
		// else
		// {
		// 	float theta = acos(dot(N, ray.dir));
		// 	if (sin(theta)*kk >= 1)
		// 		newdir = ray.dir - 2 * N * dot(N, ray.dir);
		// 	else
		// 		newdir = normalize(tan(asin(sin(theta)*kk)) * normalize(ray.dir - nap * dot(nap, ray.dir)) + normalize(nap));
		// }
}


const float magn = 16;
const int nSample = 1;
const int imageWidth = 512;
const int imageHeight = imageWidth;
char pixels[imageWidth * imageHeight * 3];

int main(int argc, char* argv[])
{
	loadShapes();
	fprintf(stderr, "%lu primitives loaded\n", objects.size());
	loadSAS();
	int byteCnt = 0;
	for (int y=0; y<imageHeight; ++y)
		for (int x=0; x<imageWidth; ++x)
		{
			vec3 camera(0,1,4);
			vec3 res(0,0,0);

			for (int i=0; i<nSample; ++i)
			{
				float w = 2.7;
				vec3 tar(-w/2 + w*(x+randf())/imageWidth, 1.0+w/2-w*(y+randf())/imageHeight, 0);
				Ray ray = {camera, normalize(tar - camera)};
				res += cast(ray);
			}
			res *= magn/nSample;
			pixels[byteCnt++] = std::min(255.0f, res.x * 255);
			pixels[byteCnt++] = std::min(255.0f, res.y * 255);
			pixels[byteCnt++] = std::min(255.0f, res.z * 255);
		}
	writeBMP("output.bmp", pixels, imageWidth, imageHeight);
	fprintf(stderr, "%lld rays casted \n", nRay);
}
