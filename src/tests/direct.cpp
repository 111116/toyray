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
		LIGHT, DIFFUSE, REFLECT, EYE
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
			fin >> origin >> radius;
			shape = new(Sphere)(origin, radius);
		}
		face::attrtype attr;
		if (strattr == "light") attr = face::LIGHT;
		if (strattr == "diffuse") attr = face::DIFFUSE;
		if (strattr == "reflect") attr = face::REFLECT;
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



vec3 sampleSphere(vec3 N)
{
	vec3 v;
	do v = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
	while (norm(v)>1 || norm(v)<1e-4);
	v = normalize(v);
	v += N;
	if (v.x==0 && v.y==0 && v.z==0)
		return sampleSphere(N);
	return normalize(v);
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

	if (bounces > 0) return vec3();

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
		shadowRay.origin += 1e-5 * ldir;
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
				* abs(dot(-ldir, Nl))
				* std::max(0.0f, dot(ldir, N))
				* (lights.size() / acosf(-1))
				* light->shape->surfaceArea()
				* pow(norm(pl - p), -2);
				// this is hemispherical integral f*cos(N,v) dS / (2pi)
				// actual radiance should be: 2/pi * int(f cos(N,v) dS

		// sample indirect illumination
		float prob = bounces<3? 1: std::max(color.x, std::max(color.y, color.z));
		// possibly terminating path
		if (randf() < prob) 
		{
			vec3 inColor = cast((Ray){p, sampleSphere(N)}, bounces+1, face::DIFFUSE);
			res += 1/prob * color * inColor;
			// expectancy of spherical estimator should be exact radiance
		}
		return res;
	}

	if (hit->attr == face::REFLECT)
		return cast((Ray){p, ray.dir - 2 * N * dot(N, ray.dir)}, bounces+1, face::REFLECT);

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


const float magn = 64;
int nSample = 16;
const int imageWidth = 512;
const int imageHeight = imageWidth;
char pixels[imageWidth * imageHeight * 3];

int main(int argc, char* argv[])
{
	if (argc>1) nSample = atoi(argv[1]);
	loadShapes();
	fprintf(stderr, "%lu primitives loaded\n", objects.size());
	loadSAS();
	int byteCnt = 0;
	long long nZero = 0, nPrimary = 0;
	fprintf(stderr, "rendering at %d x %d, %d spp\n", imageWidth, imageHeight, nSample);
	for (int y=0; y<imageHeight; ++y)
		for (int x=0; x<imageWidth; ++x)
		{
			vec3 camera(0,1,4), res;
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
	writeBMP("output_direct.bmp", pixels, imageWidth, imageHeight);
	fprintf(stderr, "%lld rays casted \n", nRay);
	fprintf(stderr, "%lld shadow rays\n", nShadow);
	fprintf(stderr, "%lld primary rays\n", nPrimary);
	fprintf(stderr, "%lld zero-radiance paths (%.2f%%)\n", nZero, (float)nZero / nPrimary * 100);
}
