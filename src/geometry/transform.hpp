#pragma once

#include "geometry.hpp"
#include "math/matfloat.hpp"

class Transformed : public Primitive
{
	Primitive* const inner;
	const mat4f trans;
	const mat4f invTrans;
	AABox bound;

public:
	// using singular matrix is strongly discouraged!
	Transformed(Primitive* inner, const mat4f& m):
		inner(inner), trans(m), invTrans(inverse(m))
	{
		// calculate bounding box of transformed shape
		// (though this bound might be looser)
		AABox t = inner->boundingVolume();
		point boundpoints[] = {
			vec3f(t.x1, t.y1, t.z1),
			vec3f(t.x1, t.y1, t.z2),
			vec3f(t.x1, t.y2, t.z1),
			vec3f(t.x1, t.y2, t.z2),
			vec3f(t.x2, t.y1, t.z1),
			vec3f(t.x2, t.y1, t.z2),
			vec3f(t.x2, t.y2, t.z1),
			vec3f(t.x2, t.y2, t.z2)
		};
		bound = AABox(transformedPoint(trans, boundpoints[0]));
		for (int i=1; i<8; ++i)
			bound = bound + transformedPoint(trans, boundpoints[i]);
	}

	bool intersect(const Ray& ray, Hit* result) const
	{
		Ray iray(transformedPoint(invTrans, ray.origin), transformedDir(invTrans, ray.dir));
		Hit iresult;
		bool b = inner->intersect(iray, &iresult);
		if (!b) return false;
		*result = Hit(
			transformedPoint(trans, iresult.p),
			transformedDir(transposed(invTrans), iresult.Ns),
			transformedDir(transposed(invTrans), iresult.Ng));
		return true;
	}

	AABox boundingVolume() const
	{
		return bound;
	}

	SampleInfo sampleSurface(Sampler& sampler) const
	{
		SampleInfo info = inner->sampleSurface(sampler);
		vec3f N1,N2;
		getLocalBasis(info.normal, N1, N2);
		return SampleInfo(transformedPoint(trans, info.p),
			transformedDir(transposed(invTrans), info.normal),
			info.pdf / fabs(norm(cross((trans*vec4f(N1,0)).xyz(), (trans*vec4f(N2,0)).xyz()))));
	}

private:
	static vec3f transformedDir(const mat4f& mat, const vec3f& v) {
		return normalized((mat * vec4f(v, 0)).xyz());
	}
	static vec3f transformedPoint(const mat4f& mat, const vec3f& v) {
		return (mat * vec4f(v, 1)).xyz();
	}
	static void getLocalBasis(const vec3f& Ns, vec3f& N1, vec3f& N2)
	{
		// generate orthonormal basis [Ns,N1,N2]
		N1 = cross(Ns,vec3f(0,0,1));
		if (norm(N1)<0.1) N1 = cross(Ns,vec3f(0,1,0));
		N1 = normalized(N1);
		N2 = cross(Ns,N1);
	}
};
