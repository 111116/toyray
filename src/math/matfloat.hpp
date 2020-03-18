// mathematics stuff
#pragma once

#include "vecfloat.hpp"

struct mat3f;
struct mat4f;



// basic definitions

struct mat3f
{
	float data[3][3];
	mat3f() {
		memset(data, 0, sizeof data);
	}
	mat3f(const vec3f& a, const vec3f& b, const vec3f& c)
	{
		data[0][0] = a.x; data[1][0] = a.y; data[2][0] = a.z;
		data[0][1] = b.x; data[1][1] = b.y; data[2][1] = b.z;
		data[0][2] = c.x; data[1][2] = c.y; data[2][2] = c.z;
	}
	float* operator[](unsigned i) {
		return data[i];
	}
	float const* operator[](unsigned i) const {
		return data[i];
	}
};

struct mat4f
{
	float data[4][4];
	mat4f() {
		memset(data, 0, sizeof data);
	}
	mat4f(const vec4f& a, const vec4f& b, const vec4f& c, const vec4f& d)
	{
		data[0][0] = a.x; data[1][0] = a.y; data[2][0] = a.z; data[3][0] = a.w;
		data[0][1] = b.x; data[1][1] = b.y; data[2][1] = b.z; data[3][1] = b.w;
		data[0][2] = c.x; data[1][2] = c.y; data[2][2] = c.z; data[3][2] = c.w;
		data[0][3] = d.x; data[1][3] = d.y; data[2][3] = d.z; data[3][3] = d.w;
	}
	mat4f(float k00, float k01, float k02, float k03,
		  float k10, float k11, float k12, float k13,
		  float k20, float k21, float k22, float k23,
		  float k30, float k31, float k32, float k33):
		data{k00,k01,k02,k03,k10,k11,k12,k13,k20,k21,k22,k23,k30,k31,k32,k33} {}
	float* operator[](unsigned i) {
		return data[i];
	}
	float const* operator[](unsigned i) const {
		return data[i];
	}
	static const mat4f unit;
};


// element-wise operations

mat3f operator + (const mat3f& a, const mat3f& b);
mat3f& operator += (mat3f& a, const mat3f& b);
mat4f operator + (const mat4f& a, const mat4f& b);
mat4f& operator += (mat4f& a, const mat4f& b);
mat3f operator - (const mat3f& a, const mat3f& b);
mat3f& operator -= (mat3f& a, const mat3f& b);
mat4f operator - (const mat4f& a, const mat4f& b);
mat4f& operator -= (mat4f& a, const mat4f& b);
mat3f operator * (float k, const mat3f& b);
mat3f& operator *= (mat3f& a, float k);
mat4f operator * (float k, const mat4f& b);
mat4f& operator *= (mat4f& a, float k);

// matrix-matrix multiplication
mat3f operator * (const mat3f& a, const mat3f& b);
mat3f& operator *= (mat3f& a, const mat3f& b);
mat4f operator * (const mat4f& a, const mat4f& b);
mat4f& operator *= (mat4f& a, const mat4f& b);

// matrix-vector multiplication
vec3f operator * (const mat3f& a, const vec3f& b);
vec4f operator * (const mat4f& a, const vec4f& b);

// other functions
mat3f transposed(const mat3f& a);
mat4f transposed(const mat4f& a);
mat3f diag(int x, int y, int z);
mat4f diag(int x, int y, int z, int w);

float det(mat3f a);
mat3f inverse(mat3f a);
mat4f inverse(mat4f a);

bool operator == (const mat4f& a, const mat4f& b);
bool operator != (const mat4f& a, const mat4f& b);

