#include <algorithm>
#include <vector>
#include <fstream>
#include "writebmp.h"
#include "geometry.h"

#define DEBUG


struct face
{
	enum attrtype
	{
		LIGHT, DIFFUSE, EYE
	};
	attrtype attr;
	Primitive* shape;
	vec3 color;
};

std::vector<face> objects, lights;
// lights specifically for LD path optimization


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
		if (strattr == "light") 
			lights.push_back((face){attr, shape, color});
	}
}



void loadSAS()
{
	fprintf(stderr, "Testing brute force. No acceleration structure.\n");
}



vec3 outDiffuse(vec3 Normal)
{
	vec3 res;
	do res = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
	while (norm(res) > 1);
	return normalize(1.00001 * Normal + res);
}


long long nRay = 0;
long long nShadow = 0;


face* hitAnything(Ray ray, std::vector<face>& objects)
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



vec3 cast(Ray ray, int bounces, face::attrtype lastType)
{
	// prevent ray intersecting its source surface
	ray.origin += 1e-5 * ray.dir;
	nRay++;

#ifdef DEBUG
	// printf("%f %f %f %f %f %f\n",ray.origin.x,ray.origin.y,ray.origin.z,ray.dir.x,ray.dir.y,ray.dir.z);
	assert(abs(norm(ray.dir) - 1) < 1e-5);
#endif

	face* hit = hitAnything(ray, objects);
	if (hit == NULL) return vec3();

	vec3 color = hit->color;

	if (hit->attr == face::LIGHT)
		return (lastType == face::DIFFUSE)? vec3(): color;
	// separate Light->Diffuse path

	point p;
	assert(hit->shape->intersect(ray, &p));
	vec3 N = hit->shape->normalAtPoint(p);

	if (hit->attr == face::DIFFUSE)
	{
		// sample direct illumination (assuming diffuse light source)
		++nShadow;
		vec3 res;
		face* light = &lights[rand() % lights.size()];
		point pl = light->shape->sampleOnSurface();
		vec3 Nl = light->shape->normalAtPoint(pl);
		vec3 ldir = normalize(pl - p);

		Ray shadowRay{p, ldir};
		shadowRay.origin += 1e-5 * shadowRay.dir;
		face* blk = hitAnything(shadowRay, objects);
		bool blocked = 0;
		if (blk != NULL)
		{
			point pb;
			blk->shape->intersect(shadowRay, &pb);
			blocked = (norm(pb-p) + 1e-5 <= norm(pl-p));
		}
		if (!blocked)
			res += light->color * color
				* std::max(0.0f, dot(-ldir, Nl))
				* std::max(0.0f, dot(ldir, N))
				* (0.5 / acos(-1) / lights.size())
				* pow(norm(pl - p), -2);

		// sample indirect illumination
		float prob = bounces<3? 1: std::max(color.x, std::max(color.y, color.z));
		// possibly terminating path
		if (randf() < prob) 
		{
			vec3 inColor = cast((Ray){p, outDiffuse(N)}, bounces+1, face::DIFFUSE);
			res += 1/prob * color * inColor;
		}
		return res;
	}

	// reflection
		// vec3 newdir = ray.dir - 2 * N * dot(N, ray.dir);
		// return cast((Ray){p, newdir});

	// refraction
		// float rI = 1.6;
		// if (dot(N, ray.dir) < 0)
		// {
		// 	float theta = acos(dot(-N, ray.dir));
		// 	newdir = normalize(tan(asin(sin(theta)/rI)) * normalize(ray.dir - N * dot(N, ray.dir)) + normalize(-N));
		// }
		// else
		// {
		// 	float theta = acos(dot(N, ray.dir));
		// 	if (sin(theta)*rI >= 1)
		// 		newdir = ray.dir - 2 * N * dot(N, ray.dir);
		// 	else
		// 		newdir = normalize(tan(asin(sin(theta)*rI)) * normalize(ray.dir - nap * dot(nap, ray.dir)) + normalize(nap));
		// }
}


const float magn = 16;
const int nSample = 256;
const int imageWidth = 512;
const int imageHeight = imageWidth;
char pixels[imageWidth * imageHeight * 3];

int main(int argc, char* argv[])
{
	loadShapes();
	fprintf(stderr, "%lu primitives loaded\n", objects.size());
	loadSAS();
	int byteCnt = 0;
	long long nZero = 0, nPrimary = 0;
	fprintf(stderr, "rendering at %d x %d, %d spp\n", imageWidth, imageHeight, nSample);
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
				vec3 sample = cast(ray, 0, face::EYE);
				nPrimary += 1;
				nZero += (sample.x==0 && sample.y==0 && sample.z==0);
				res += sample;
			}
			res *= magn / nSample;
			pixels[byteCnt++] = std::min(255.0f, res.x * 255);
			pixels[byteCnt++] = std::min(255.0f, res.y * 255);
			pixels[byteCnt++] = std::min(255.0f, res.z * 255);
		}
	writeBMP("output.bmp", pixels, imageWidth, imageHeight);
	fprintf(stderr, "%lld rays casted \n", nRay);
	fprintf(stderr, "%lld shadow rays\n", nShadow);
	fprintf(stderr, "%lld primary rays\n", nPrimary);
	fprintf(stderr, "%lld zero-radiance paths (%.2f%%)\n", nZero, (float)nZero / nPrimary * 100);
}
