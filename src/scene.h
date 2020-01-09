#include <fstream>
#include <iostream>
#include <string>

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

struct Scene
{
	std::vector<face> objects, lights;
	// lights specifically for Light-Diffuse path optimization
	
};

void loadShapes(const char* infilename)
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
