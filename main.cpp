#include <algorithm>
#include <vector>
#include <fstream>
#include "writebmp.h"
#include "geometry.h"

#define DEBUG


Sphere ball(vec3(0,0,0), 1);
point source(0,1.989,0);


struct face
{
	Primitive* shape;
	vec3 diffuseColor;
};

std::vector<face> objects;


void loadShapes()
{
	std::ifstream fin("mesh");
	std::string type;
	while (fin >> type)
	{
		if (type == "triangle")
		{
			float x,y,z;
			Triangle* t = new(Triangle);
			fin >> x >> y >> z;
			t->v1 = vec3(x,y,z);
			fin >> x >> y >> z;
			t->v2 = vec3(x,y,z);
			fin >> x >> y >> z;
			t->v3 = vec3(x,y,z);
			fin >> x >> y >> z;
			t->vn1 = vec3(x,y,z);
			t->vn2 = vec3(x,y,z);
			t->vn3 = vec3(x,y,z);
			t->preprocess();
			fin >> x >> y >> z;
			vec3 color(x,y,z);
			objects.push_back((face){t,color});
		}
		if (type == "sphere")
		{
			float x,y,z,r;
			std::cin >> x >> y >> z >> r;
			Sphere* t = new(Sphere)(vec3(x,y,z),r);
			fin >> x >> y >> z;
			vec3 color(x,y,z);
			objects.push_back((face){t,color});
		}
		if (type == "exit")
			return;
	}
}

void loadSAS()
{
	// testing brute force. no acceleration structure
}


long long nRay = 0;


face* hitAnything(Ray ray)
{
	ray.origin += 1e-5 * ray.dir;

	face* hit = NULL;
	float dist;
	for (face& shape: objects)
	{
		point res;
		if (shape.shape->intersect(ray, &res))
		{
			if (hit == NULL || dist > norm(res - ray.origin))
			{
				hit = &shape;
				dist = norm(res - ray.origin);
			}
		}
	}
	return hit;
}


vec3 cast(Ray ray)
{
	// count number of ray casted
	nRay++;
	// prevent ray intersecting its source surface

	// printf("%f %f %f %f %f %f\n",ray.origin.x,ray.origin.y,ray.origin.z,ray.dir.x,ray.dir.y,ray.dir.z);

#ifdef DEBUG
	assert(abs(norm(ray.dir) - 1) < 1e-5);
#endif

	face* hit = hitAnything(ray);
	// bruteforcing checking against every primitive
	if (hit == NULL)
		return vec3(0,0,0);

	point p;
	hit->shape->intersect(ray, &p);
	vec3 lightSourceDir = normalize(source - p);
	
	Ray shadowRay {p, normalize(source - p)};
	face* block = hitAnything(shadowRay);
	bool blocked = false;
	if (block != NULL)
	{
		point pb;
		block->shape->intersect(shadowRay, &pb);
		if (norm(pb - p) < norm(source - p))
			blocked = true;
	}
	vec3 res(0,0,0);
	if (!blocked)
		//return vec3(0,0,1);
		res = hit->diffuseColor * std::max(0.0f,dot(lightSourceDir, hit->shape->normalAtPoint(p))) * 1.5 * pow(norm(source - p), -2);
	
	if (res.x > 1) res.x = 1;
	if (res.y > 1) res.y = 1;
	if (res.z > 1) res.z = 1;
	return res;

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
		// 	newdir = normalize(tan(asin(sin(theta)/kk)) * normalize(ray.dir + N * dot(-N, ray.dir)) + normalize(-N));
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


const int imageWidth = 512;
const int imageHeight = imageWidth;
char pixels[imageWidth * imageHeight * 3];

int main(int argc, char* argv[])
{
	loadShapes();
	loadSAS();
	int byteCnt = 0;
	for (int y=0; y<imageHeight; ++y)
		for (int x=0; x<imageWidth; ++x)
		{
			vec3 camera(0,1,3.7);
			vec3 res(0,0,0);

			int nSample = 64;
			for (int i=0; i<nSample; ++i)
			{
				float w = 3;
				vec3 tar(-w/2 + w*(x+(float)rand()/RAND_MAX)/imageWidth, 1.0+w/2-w*(y+(float)rand()/RAND_MAX)/imageHeight, 0);
				Ray ray = {camera, normalize(tar - camera)};

				// manualy rotate camera
				/*
				float theta = -0.4;
				float x = ray.origin.x * cos(theta) - ray.origin.y * (-sin(theta));
				float y = ray.origin.y * cos(theta) + ray.origin.x * sin(theta);
				ray.origin.x = x;
				ray.origin.y = y;
				x = ray.dir.x * cos(theta) - ray.dir.y * sin(theta);
				y = ray.dir.y * cos(theta) + ray.dir.x * sin(theta);
				ray.dir.x = x;
				ray.dir.y = y;

				theta = -0.2;
				x = ray.origin.x * cos(theta) - ray.origin.z * (-sin(theta));
				float z = ray.origin.z * cos(theta) + ray.origin.x * sin(theta);
				ray.origin.x = x;
				ray.origin.z = z;
				x = ray.dir.x * cos(theta) - ray.dir.z * sin(theta);
				z = ray.dir.z * cos(theta) + ray.dir.x * sin(theta);
				ray.dir.x = x;
				ray.dir.z = z;
				*/
				source.z = (float)rand()/RAND_MAX/2-0.25;
				source.x = (float)rand()/RAND_MAX/2-0.25;
				res += cast(ray);
			}
			res *= 1.0/nSample;
			pixels[byteCnt++] = res.x * 255;
			pixels[byteCnt++] = res.y * 255;
			pixels[byteCnt++] = res.z * 255;
		}
	writeBMP("output.bmp", pixels, imageWidth, imageHeight);
	fprintf(stderr, "%lld rays casted \n", nRay);
}
