#include <algorithm>
#include "writebmp.h"
#include "geometry.h"



void loadShapes()
{
	
}

void loadSAS()
{
	
}


int nCameraRay = 0;

vec3 cast(Ray ray)
{
	nCameraRay++;
	vec3 lightSourceDir = normalize(vec3(-0.4,0.8,1.0));
	Sphere ball(vec3(0,0,0), 1);
	if (!ball.hasIntersection(ray)) return vec3(0,0,0);
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
			pixels[byteCnt++] = res.x * 255;
			pixels[byteCnt++] = res.y * 255;
			pixels[byteCnt++] = res.z * 255;
		}
	writeBMP("output.bmp", pixels, imageWidth, imageHeight);
	fprintf(stderr, "%d camera rays casted \n", nCameraRay);
}
