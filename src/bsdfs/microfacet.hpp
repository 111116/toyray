#pragma once

#include "math/vecfloat.hpp"
#include <functional>

// behind the pixels
// https://zhuanlan.zhihu.com/p/20119162

namespace EDX {

inline float CosTheta2(const vec3f& vec) { return vec.z * vec.z; }
inline float CosTheta(const vec3f& vec) { return vec.z; }
inline float TanTheta(const vec3f& vec)
{
	float temp = 1 - vec.z * vec.z;
	if (temp <= 0.0f)
		return 0.0f;
	return sqrtf(temp) / vec.z;
}
inline float TanTheta2(const vec3f& vec)
{
	float temp = 1 - vec.z * vec.z;
	if (temp <= 0.0f)
		return 0.0f;
	return temp / (vec.z * vec.z);
}

inline float GGX_D(vec3f wm, float alpha) // alpha为粗糙度
{
    float tanTheta2 = TanTheta2(wm);
	float cosTheta2 = CosTheta2(wm);

	float root = alpha / (cosTheta2 * (alpha * alpha + tanTheta2));

	return 1/PI * (root * root);
}

inline float Smith_G1(vec3f v, vec3f wm, float alpha)
{
    if (wm.z < 0) wm = -wm;

    float tanTheta = fabs(TanTheta(v));

    if (tanTheta == 0.0f)
        return 1.0f;

    if (dot(v, wm) * CosTheta(v) <= 0)
        return 0.0f;

    float root = alpha * tanTheta;
    return 2.0f / (1.0f + sqrtf(1.0f + root*root));
}

inline float Smith_G(vec3f wo, vec3f wi, vec3f wm, float alpha)
{
	return Smith_G1(wo, wm, alpha) * Smith_G1(wi, wm, alpha);
}

}


// https://schuttejoe.github.io/post/ggximportancesamplingpart2/
namespace schuttejoe {

//====================================================================
// https://hal.archives-ouvertes.fr/hal-01509746/document
vec3f GgxVndf(vec3f wo, float roughness, float u1, float u2)
{
    float ttt = 1;
    if (wo.z < 0) {
        wo.z = -wo.z;
        ttt = -1;
    }
    // -- Stretch the view vector so we are sampling as though
    // -- roughness==1
    vec3f v = normalized(vec3f(wo.x * roughness,
                                wo.y * roughness,
                                wo.z ));

    // -- Build an orthonormal basis with v, t1, and t2
    vec3f t1 = (v.y < 0.999f) ? normalized(cross(v, vec3f(0,1,0))) : vec3f(1,0,0);
    vec3f t2 = cross(t1, v);

    // -- Choose a point on a disk with each half of the disk weighted
    // -- proportionally to its projection onto direction v
    float a = 1.0f / (1.0f + v.z);
    float r = sqrtf(u1);
    float phi = (u2 < a) ? (u2 / a) * PI 
                         : PI + (u2 - a) / (1.0f - a) * PI;
    float p1 = r * cos(phi);
    float p2 = r * sin(phi) * ((u2 < a) ? 1.0f : v.z);

    // -- Calculate the normal in this stretched tangent space
    vec3f n = p1 * t1 + p2 * t2
             + sqrtf(fmax(0.0f, 1.0f - p1 * p1 - p2 * p2)) * v;

    // -- unstretch and normalize the normal
    return normalized(vec3f(roughness * n.x,
                            roughness * n.y,
                            fmax(0.0f, n.z) * ttt));
}

}