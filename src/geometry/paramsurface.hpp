#pragma once

#include "geometry.hpp"
#include "trianglemesh.hpp"

class ParamSurface: public Primitive
{
	BasicContainer* mesh;
protected:
	virtual vec3f surface(float u, float v, vec3f& dpdu, vec3f& dpdv) const = 0;
	void buildmesh()
	{
		// build approximating mesh
		float step = 0.01;
		std::vector<BasicPrimitive*> trigs;
		for (float i=0; i+step/2<1; i+=step)
			for (float j=0; j+step/2<1; j+=step)
			{
				vec3f t1,t2,t; // dummy vars
				vec3f p00 = surface(i,j,t1,t2);
				vec3f p01 = surface(i,j+step,t1,t2);
				vec3f p10 = surface(i+step,j,t1,t2);
				vec3f p11 = surface(i+step,j+step,t1,t2);
				vec2f u00 = vec2f(i,j);
				vec2f u01 = vec2f(i,j+step);
				vec2f u10 = vec2f(i+step,j);
				vec2f u11 = vec2f(i+step,j+step);
				if (norm(cross(p00-p01,p00-p10)) > 1e-12)
				trigs.push_back(new Triangle(p00,p10,p01,u00,u10,u01,t,t,t));
				if (norm(cross(p00-p01,p00-p10)) > 1e-12)
				dynamic_cast<Triangle*>(trigs.back())->recompute_normal();
				if (norm(cross(p11-p01,p11-p10)) > 1e-12)
				trigs.push_back(new Triangle(p10,p11,p01,u10,u11,u01,t,t,t));
				if (norm(cross(p11-p01,p11-p10)) > 1e-12)
				dynamic_cast<Triangle*>(trigs.back())->recompute_normal();
			}
		mesh = new BasicContainer(trigs);
	}

public:
	ParamSurface(){}

	bool intersect(const Ray& ray, Hit* result) const
	{
		Hit tmpresult;
		Ray forwardray (ray.atParam(1e-2), ray.dir);
		bool boolres = mesh->intersect(forwardray, &tmpresult);
		if (!boolres) return false;
		// determine initial point
		float t = norm(tmpresult.p - ray.origin);
		float u = tmpresult.uv.x;
		float v = tmpresult.uv.y;
		u = fmin(1, fmax(0, u));
		v = fmin(1, fmax(0, v));
		vec3f p, dpdu, dpdv;
		// solve eqn(t,u,v): f(u,v) - origin - t * dir = 0
		for (int _=0; _<5; ++_)
		{
			p = surface(u,v,dpdu,dpdv) - ray.atParam(t);
			// vec3f t1,t2;
			// auto dpdu1 = (surface(u+1e-4,v,t1,t2)-surface(u,v,dpdu,dpdv))/1e-4;
			// auto dpdv1 = (surface(u,v+1e-4,t1,t2)-surface(u,v,dpdu,dpdv))/1e-4;
			// console.log(u,v);
			// console.log(dpdu, dpdv);
			// console.log(dpdu1, dpdv1);
			// console.log(norm(dpdu-dpdu1)/norm(dpdu), norm(dpdv-dpdv1)/norm(dpdv));
			// console.log(_,":",norm(p),",  ",t,u,v);
			// for (int i=0; i<3; ++i) for (int j=0; j<3; ++j)
			// console.log((inverse(mat3f(-ray.dir, dpdu, dpdv))*mat3f(-ray.dir, dpdu, dpdv))[i][j]);
			vec3f delt = inverse(mat3f(-ray.dir, dpdu, dpdv)) * p;
			t -= delt.x;
			u = fmin(1, fmax(0, u - delt.y));
			// v = v - delt.z;
			v = fmin(1, fmax(0, v - delt.z));
		}
		p = surface(u,v,dpdu,dpdv);
		if (norm(p-ray.atParam(t)) < 1e-6 && t>0) {
			vec3f N = normalized(cross(dpdu,dpdv));
			*result = {p,N,N,vec2f(u,v)};
			return true;
		}
		return false;
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		throw "not implemented";
	}

	AABox boundingVolume() const
	{
		// not implemented
		return AABox::infAAB;
	}
};


