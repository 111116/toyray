// This is a naive path tracing implementation
// using a simple sample strategy
// with no threading and no spatial acceleration structure,
// supporting only perfect diffuse / reflective surfaces and glass (not physically based),
// with area diffuse light sources.


#include <algorithm>
#include <vector>
#include <fstream>
#include "geometry.h"
#include "envmap.h"
// #include "writebmp.h"
#include "saveexr.h"

#define DEBUG


struct face
{
	enum attrtype
	{
		LIGHT, DIFFUSE, REFLECT, REFRACT
	};
	attrtype attr;
	Primitive* shape;
	vec3 color;
};

std::vector<face> objects, lights;
// lights specifically for Light-Diffuse path optimization

char infilename[] = "../cbox_specular";
char outfilename[] = "output.exr";
char hdrfilename[] = "res/uffizi.hdr";

void loadShapes()
{
	std::ifstream fin(infilename);
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
		if (strattr == "refract") attr = face::REFRACT;
		vec3 color;
		fin >> color;
		objects.push_back((face){attr, shape, color});
		if (strattr == "light") 
			lights.push_back((face){attr, shape, color});
	}
}



vec3 sampleSphere(vec3 N)
{
	vec3 v;
	while (norm(v) < 1e-5)
	{
		do v = vec3(randf()*2-1, randf()*2-1, randf()*2-1);
		while (norm(v)>1 || norm(v)<1e-4);
		v = normalize(v) + N;
	}
	return normalize(v);
}


long long nRay = 0;
long long nZero = 0;
long long nShadowRay = 0;
long long nIntersectTest = 0;


face* hitAnything(Ray ray, std::vector<face>& objects)
{
	face* hit = NULL;
	float dist;
	point res;
	// bruteforcing checking against every primitive
	nIntersectTest += objects.size();
	for (face& shape: objects)
		if (shape.shape->intersect(ray, &res))
			if (hit == NULL || dist > norm(res - ray.origin))
				hit = &shape,
				dist = norm(res - ray.origin);
	return hit;
}

Envmap env(hdrfilename);




vec3 cast(Ray ray, int bounces, bool allowLight = true)
{
	if (bounces > 1000)
	{
		fprintf(stderr, "WARNING: bounces > 1000. Skipping...\n");
		return vec3();
	}
	// prevent ray intersecting its source surface
	ray.origin += 1e-5 * ray.dir;
	nRay++;

#ifdef DEBUG
	assert(abs(norm(ray.dir) - 1) < 1e-5);
#endif 

	face* hit = hitAnything(ray, objects);
	if (hit == NULL)
	{
		// ++nZero;
		return env.envlight(ray.dir);
		// return vec3();
	}
	vec3 color = hit->color;
	if (hit->attr == face::LIGHT)
		return allowLight? color: vec3();

	point p;
	++nIntersectTest;
	hit->shape->intersect(ray, &p);
	vec3 N = hit->shape->normalAtPoint(p);

	if (hit->attr == face::DIFFUSE) // assuming lambertian
	{
		// sample direct illumination (assuming diffuse light source)
		// Note: this method works well for light sources covering a small solid angle,
		// but may increase variance otherwise
		vec3 res;
		if (!lights.empty())
		{
			face* light = &lights[rand() % lights.size()];
			point pl = light->shape->sampleOnSurface();
			vec3 Nl = light->shape->normalAtPoint(pl);
			vec3 ldir = normalize(pl - p);

			++nShadowRay;
			Ray shadowRay{p, ldir};
			shadowRay.origin += 1e-5 * ldir;
			face* blk = hitAnything(shadowRay, objects);
			bool blocked = 0;
			if (blk != NULL)
			{
				point pb;
				++nIntersectTest;
				blk->shape->intersect(shadowRay, &pb);
				blocked = (norm(pb-p) + 1e-5 <= norm(pl-p));
			}
			if (!blocked)
				res += light->color * color
					* std::max(0.0f, dot(-ldir, Nl))
					* std::max(0.0f, dot(ldir, N))
					* (lights.size() / acosf(-1))
					* light->shape->surfaceArea()
					* pow(norm(pl - p), -2);
		}

		// sample indirect illumination
		float prob = bounces<3? 1: std::max(color.x, std::max(color.y, color.z));
		// possibly terminating path
		if (randf() < prob) 
		{
			vec3 inColor = cast((Ray){p, sampleSphere(N)}, bounces+1, false);
			// split for shadowed area
			if (bounces<1 && res.x==0 && res.y==0 && res.z==0)
			{
				inColor += cast((Ray){p, sampleSphere(N)}, bounces+1, false);
				inColor *= 0.5;
			}
			res += 1/prob * color * inColor;
		}
		else
			++nZero;
		return res;
	}

	if (hit->attr == face::REFLECT) // assuming perfect mirror
		return cast((Ray){p, normalize(ray.dir - 2 * N * dot(N, ray.dir))}, bounces+1);

	if (hit->attr == face::REFRACT) // glass, with refraction index 1.7
	{
		float reflectance = 0.1;
		float rI = 1.7;
		float fresnel = reflectance + (1.0f - reflectance) * pow(1-abs(dot(N, ray.dir)), 5);
		// fresnel reflection. Thanks to Edward Liu's article on texture rendering
		vec3 newdir;
		float sint = sin(acos(dot(N, ray.dir)));
		if (randf() < fresnel)
			newdir = normalize(ray.dir - 2 * N * dot(N, ray.dir));
		else
		{
			if (dot(N, ray.dir) < 0)
			{
				newdir = normalize(tan(asin(sint/rI)) * normalize(ray.dir - N * dot(N, ray.dir)) + normalize(-N));
			}
			else
			{
				if (sint*rI >= 1)
					newdir = normalize(ray.dir - 2 * N * dot(N, ray.dir));
				else
					newdir = normalize(tan(asin(sint*rI)) * normalize(ray.dir - N * dot(N, ray.dir)) + normalize(N));
			}
		}
		// deal with NANs. I don't know what happened. so just ignore them...
		if (std::isnan(newdir.x)  || std::isnan(newdir.y) || std::isnan(newdir.z))
		{
			fprintf(stderr, "ERROR: ray direction NAN. Skipping...\n");
			return vec3();
		}
		return randf()<0.96? cast((Ray){p, newdir}, bounces+1): vec3();
		// this might be problematic
		// light absorption should be relavent to distance traveled
	}
}


int nspp = 4; // default number of samples per pixel, may be overrided in parameters
const float magn = 1; // exposure magnification
const int imageWidth = 512;
const int imageHeight = imageWidth;
vec3 camera(0,1,4);
float cropx = 0.27, cropw = 0.25;
float cropy = 0.55, croph = 0.25;
float pixels[imageWidth * imageHeight * 3];

// note: ./test 64 SEGMENTATION FAULT! DO NOT CHANGE CODE!


int main(int argc, char* argv[])
{
#ifdef DEBUG
	fprintf(stderr, "WARNING: running in debug mode.\n");
#endif
	if (argc>1) nspp = atoi(argv[1]);
	loadShapes();
	fprintf(stderr, "INFO: %lu primitives loaded\n", objects.size());
	int byteCnt = 0;
	long long nPrimary = 0;
	fprintf(stderr, "INFO: rendering at %d x %d, %d spp\n", imageWidth, imageHeight, nspp);
	
	for (int y=0; y<imageHeight; ++y)
	{
		for (int x=0; x<imageWidth; ++x)
		{
			vec3 res;
			for (int i=0; i<nspp; ++i)
			{
				// manual camera setup
				float w = 4;
				vec3 tar(-w/2 + w*(cropx + cropw * (x+randf())/imageWidth),
					1+w/2 - w*(cropy + croph * (y+randf())/imageHeight) , 0);
				Ray ray = {camera, normalize(tar - camera)};
				res += cast(ray, 0);
				nPrimary += 1;
			}
			res *= magn / nspp;
			pixels[byteCnt++] = res.x;
			pixels[byteCnt++] = res.y;
			pixels[byteCnt++] = res.z;
		}
		fprintf(stderr, "\r%.1f%%", 100.0f*(y+1)/imageHeight);
	}
	fprintf(stderr, "\rINFO: %lld rays casted \n", nRay);
	fprintf(stderr, "INFO: %lld shadow rays\n", nShadowRay);
	fprintf(stderr, "INFO: %lld intersection tests\n", nIntersectTest);
	fprintf(stderr, "INFO: %lld primary rays\n", nPrimary);
	fprintf(stderr, "INFO: %lld zero-radiance paths (%.2f%%)\n", nZero, (float)nZero / nPrimary * 100);
	fprintf(stderr, "INFO: Writing result to %s\n", outfilename);
	SaveEXR(pixels, imageWidth, imageHeight, outfilename);
	// writeBMP("output.bmp", pixels, imageWidth, imageHeight);
}
