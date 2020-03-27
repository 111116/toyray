#pragma once

#include "math/vecfloat.hpp"

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

inline float Smith_G(vec3f wo, vec3f wi, vec3f wm, float alpha)
{
	if (wm.z < 0) wm = -wm;
	auto SmithG1 = [&](vec3f v, vec3f wm)
	{
		float tanTheta = fabs(TanTheta(v));

		if (tanTheta == 0.0f)
			return 1.0f;

		if (dot(v, wm) * CosTheta(v) <= 0)
			return 0.0f;

		float root = alpha * tanTheta;
		return 2.0f / (1.0f + sqrtf(1.0f + root*root));
	};

	return SmithG1(wo, wm) * SmithG1(wi, wm);
}

}