#ifndef PLANE_H
#define PLANE_H

#include "geometry.hpp"

// infinite plane of ax+by+cz=d where (a,b,c) is normalized

class Plane : public BasicPrimitive
{
protected:
    vec3f abc;
    float d;
public:
    Plane(const vec3f& normal, float d) : abc(normal), d(d) {}

    bool intersect(const Ray& ray, float& result) const {
        float lambda = (d - dot(abc, ray.origin)) / dot(abc, ray.dir);
        if (lambda >= 0) {
        	result = lambda;
        	return true;
        }
        return false;
    }

    vec3f Ns(const point& p) const
    {
        return abc;
    }

    vec3f Ng(const point& p) const
    {
        return abc;
    }

    AABox boundingVolume() const
    {
        return AABox::infAAB;
    }
};

#endif //PLANE_H
		

