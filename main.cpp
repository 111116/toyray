#include <algorithm>
#include "writebmp.h"
#include "geometry.h"



void loadShapes()
{
	
}

void loadSAS()
{
	
}


int nRay = 0;

vec3 cast(Ray ray)
{
	ray.origin += 1e-5 * ray.dir;
	// printf("%f %f %f %f %f %f\n",ray.origin.x,ray.origin.y,ray.origin.z,ray.dir.x,ray.dir.y,ray.dir.z);
	// if (rand()%100==0) return vec3(1,0,0);
	nRay++;
	vec3 lightSourceDir = normalize(vec3(-0.4,0.8,1.0));
	Sphere ball(vec3(0,0,0), 1);
	if (!ball.hasIntersection(ray))
	{
		if (ray.dir.y < 0)
		{
			float y0 = -2;
			float k = (y0 - ray.origin.y) / ray.dir.y;
			float x0 = ray.origin.x + k * ray.dir.x;
			float z0 = ray.origin.z + k * ray.dir.z;
			if ((int(floor(x0)) + int(floor(z0))) % 2 == 0)
				return vec3(0,0,0);
			else
			{
				float k = exp(-norm(vec3(x0,y0,z0))/100);
				if (x0>=2 && z0>=0) return vec3(0.4,0.8,1.0)*k;
				if (x0<=2 && z0<=0) return vec3(0.8,0.3,1.0)*k;
				if (x0<=2 && z0>=0) return vec3(1.0,0.7,0.2)*k;
				if (x0>=2 && z0<=0) return vec3(0.2,0.9,0.5)*k;
				return vec3(0.4*(x0<0),0.8,0.4*(z0<0));
			}
		}
		return vec3(0,0,0);
	}
	point p = ball.intersection(ray);
	vec3 nap = ball.normalAtPoint(p);
	vec3 newdir;
	// return vec3(0.4,0.8,1.0);
	float kk = 1.6;
	if (dot(nap, ray.dir) < 0)
	{
		float theta = acos(dot(-nap, ray.dir));
		// newdir = ray.dir;
		newdir = normalize(tan(asin(sin(theta)/kk)) * normalize(ray.dir + nap * dot(-nap, ray.dir)) + normalize(-nap));
		
	}
	else
	{
		float theta = acos(dot(nap, ray.dir));
		if (sin(theta)*kk >= 1)
		{
			// return vec3(0.4,0.8,1.0);
			newdir = ray.dir - 2 * nap * dot(nap, ray.dir);
		}
		else
			// newdir = ray.dir;
			newdir = normalize(tan(asin(sin(theta)*kk)) * normalize(ray.dir - nap * dot(nap, ray.dir)) + normalize(nap));
	}
	bool black = 0;
	if (rand()%100 < 5.5)
		black = 1;
	if (dot(nap, ray.dir) < 0 && (float)rand()/RAND_MAX < 0.3 * pow(norm(cross(ray.dir, nap)), 50))
		newdir = ray.dir - 2 * nap * dot(nap, ray.dir), black = 0;
	if (rand()%100 < 10)
		newdir = ray.dir - 2 * nap * dot(nap, ray.dir), black = 0;
	if (black)
		return vec3(0,0,0);
	// return 0.8 * cast((Ray){p,ray.dir+2*nap});
	return cast((Ray){p,newdir});
	float brightness = std::max(0.0f, dot(ball.normalAtPoint(ball.intersection(ray)), lightSourceDir));
	vec3 color(0.4,0.8,1.0);
	return brightness * color;
}


const int imageWidth = 512;
const int imageHeight = 512;
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
			/*
			float offsetx[] = {0.4, 0.8, 0.2, 0.6};
			float offsety[] = {0.2, 0.4, 0.6, 0.8};
			for (int i=0; i<4; ++i)
			{
				vec3 tar(0, 2.0-4.0*(y+offsety[i])/imageHeight,
						 -2.0+4.0*(x+offsetx[i])/imageWidth);
				Ray ray = {camera, normalize(tar - camera)};
				res += cast(ray);
			}
			res *= 0.25;
			*/
			int nSample = 256;
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
	fprintf(stderr, "%d rays casted \n", nRay);
}
