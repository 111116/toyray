#include <algorithm>
#include <vector>
#include <fstream>
#include "writebmp.h"
#include "geometry.h"

#define DEBUG


vec3 lightSourceDir = normalize(vec3(-0.4,0.8,1.0));
Sphere ball(vec3(0,0,0), 1);


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
	}
}

void loadSAS()
{
	// testing brute force. no acceleration structure
}


long long nRay = 0;


vec3 cast(Ray ray)
{
	// count number of ray casted
	nRay++;
	// prevent ray intersecting its source surface
	ray.origin += 1e-5 * ray.dir;

	// printf("%f %f %f %f %f %f\n",ray.origin.x,ray.origin.y,ray.origin.z,ray.dir.x,ray.dir.y,ray.dir.z);

#ifdef DEBUG
	assert(abs(norm(ray.dir) - 1) < 1e-5);
#endif

	// bruteforcing checking against every primitive
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
	if (hit != NULL)
	{
		return hit->diffuseColor * dot(lightSourceDir, hit->shape->normalAtPoint(ray.origin + dist * ray.dir));
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
			vec3 camera(-8,0,0);
			vec3 res(0,0,0);

			int nSample = 4;
			for (int i=0; i<nSample; ++i)
			{
				vec3 tar(0, 2.0-4.0*(y+(float)rand()/RAND_MAX)/imageHeight,
						 -2.0+4.0*(x+(float)rand()/RAND_MAX)/imageWidth);
				Ray ray = {camera, normalize(tar - camera)};

				// manualy rotate camera
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
