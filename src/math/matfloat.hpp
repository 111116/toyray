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

const mat4f mat4f::unit(vec4f(1,0,0,0),vec4f(0,1,0,0),vec4f(0,0,1,0),vec4f(0,0,0,1));

// element-wise operations

// addition
mat3f operator + (const mat3f& a, const mat3f& b)
{
	mat3f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = a[i][j] + b[i][j];
	return res;
}
mat3f& operator += (mat3f& a, const mat3f& b)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] += b[i][j];
	return a;
}
mat4f operator + (const mat4f& a, const mat4f& b)
{
	mat4f res;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			res[i][j] = a[i][j] + b[i][j];
	return res;
}
mat4f& operator += (mat4f& a, const mat4f& b)
{
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			a[i][j] += b[i][j];
	return a;
}
// subtraction
mat3f operator - (const mat3f& a, const mat3f& b)
{
	mat3f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = a[i][j] - b[i][j];
	return res;
}
mat3f& operator -= (mat3f& a, const mat3f& b)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] -= b[i][j];
	return a;
}
mat4f operator - (const mat4f& a, const mat4f& b)
{
	mat4f res;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			res[i][j] = a[i][j] - b[i][j];
	return res;
}
mat4f& operator -= (mat4f& a, const mat4f& b)
{
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			a[i][j] -= b[i][j];
	return a;
}
// const multiplication
mat3f operator * (float k, const mat3f& b)
{
	mat3f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = k * b[i][j];
	return res;
}
mat3f& operator *= (mat3f& a, float k)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] *= k;
	return a;
}
mat4f operator * (float k, const mat4f& b)
{
	mat4f res;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			res[i][j] = k * b[i][j];
	return res;
}
mat4f& operator *= (mat4f& a, float k)
{
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			a[i][j] *= k;
	return a;
}



// matrix-matrix multiplication

mat3f operator * (const mat3f& a, const mat3f& b)
{
	mat3f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			for (int k=0; k<3; ++k)
				res[i][j] += a[i][k] * b[k][j];
	return res;
}
mat3f& operator *= (mat3f& a, const mat3f& b) {
	return a = a * b;
}
mat4f operator * (const mat4f& a, const mat4f& b)
{
	mat4f res;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			for (int k=0; k<4; ++k)
				res[i][j] += a[i][k] * b[k][j];
	return res;
}
mat4f& operator *= (mat4f& a, const mat4f& b) {
	return a = a * b;
}



// matrix-vector multiplication

vec3f operator * (const mat3f& a, const vec3f& b)
{
	return vec3f(
		a[0][0] * b.x + a[0][1] * b.y + a[0][2] * b.z,
		a[1][0] * b.x + a[1][1] * b.y + a[1][2] * b.z,
		a[2][0] * b.x + a[2][1] * b.y + a[2][2] * b.z
	);
}
vec4f operator * (const mat4f& a, const vec4f& b)
{
	return vec4f(
		a[0][0] * b.x + a[0][1] * b.y + a[0][2] * b.z + a[0][3] * b.w,
		a[1][0] * b.x + a[1][1] * b.y + a[1][2] * b.z + a[1][3] * b.w,
		a[2][0] * b.x + a[2][1] * b.y + a[2][2] * b.z + a[2][3] * b.w,
		a[3][0] * b.x + a[3][1] * b.y + a[3][2] * b.z + a[3][3] * b.w
	);
}


// other functions

mat3f transposed(const mat3f& a) {
	return mat3f(
		vec3f(a[0][0], a[0][1], a[0][2]),
		vec3f(a[1][0], a[1][1], a[1][2]),
		vec3f(a[2][0], a[2][1], a[2][2])
	);
}
mat4f transposed(const mat4f& a) {
	return mat4f(
		vec4f(a[0][0], a[0][1], a[0][2], a[0][3]),
		vec4f(a[1][0], a[1][1], a[1][2], a[1][3]),
		vec4f(a[2][0], a[2][1], a[2][2], a[2][3]),
		vec4f(a[3][0], a[3][1], a[3][2], a[3][3])
	);
}

mat3f diag(int x, int y, int z)
{
	mat3f res;
	res[0][0] = x;
	res[1][1] = y;
	res[2][2] = z;
	return res;
}
mat4f diag(int x, int y, int z, int w)
{
	mat4f res;
	res[0][0] = x;
	res[1][1] = y;
	res[2][2] = z;
	res[3][3] = z;
	return res;
}


float det(mat3f a)
{
	return
		+ a[0][0] * a[1][1] * a[2][2]
		+ a[0][1] * a[1][2] * a[2][0]
		+ a[0][2] * a[1][0] * a[2][1]
		- a[0][0] * a[1][2] * a[2][1]
		- a[0][1] * a[1][0] * a[2][2]
		- a[0][2] * a[1][1] * a[2][0];
}

mat3f inverse(mat3f a)
{
	float invdet = 1/det(a);
	mat3f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[j][i] = (a[(i+1)%3][(j+1)%3] * a[(i+2)%3][(j+2)%3] - a[(i+2)%3][(j+1)%3] * a[(i+1)%3][(j+2)%3]) * invdet;
	return res;
}

mat4f inverse(mat4f a);

bool operator == (const mat4f& a, const mat4f& b) {
	return a[0][0]==b[0][0] &&
		a[0][1]==b[0][1] &&
		a[0][2]==b[0][2] &&
		a[0][3]==b[0][3] &&
		a[1][0]==b[1][0] &&
		a[1][1]==b[1][1] &&
		a[1][2]==b[1][2] &&
		a[1][3]==b[1][3] &&
		a[2][0]==b[2][0] &&
		a[2][1]==b[2][1] &&
		a[2][2]==b[2][2] &&
		a[2][3]==b[2][3] &&
		a[3][0]==b[3][0] &&
		a[3][1]==b[3][1] &&
		a[3][2]==b[3][2] &&
		a[3][3]==b[3][3];
}

bool operator != (const mat4f& a, const mat4f& b) {
	return !(a==b);
}


// 4x4 matrix inversion implementation below

// CONVERTED from niswegmann/small-matrix-inverse/invert4x4_c.h UNLICENSED

namespace
{
static inline void invert4x4(const float * src, float * dst)
{
    float det;

    /* Compute adjoint: */

    dst[0] =
        + src[ 5] * src[10] * src[15]
        - src[ 5] * src[11] * src[14]
        - src[ 9] * src[ 6] * src[15]
        + src[ 9] * src[ 7] * src[14]
        + src[13] * src[ 6] * src[11]
        - src[13] * src[ 7] * src[10];

    dst[1] =
        - src[ 1] * src[10] * src[15]
        + src[ 1] * src[11] * src[14]
        + src[ 9] * src[ 2] * src[15]
        - src[ 9] * src[ 3] * src[14]
        - src[13] * src[ 2] * src[11]
        + src[13] * src[ 3] * src[10];

    dst[2] =
        + src[ 1] * src[ 6] * src[15]
        - src[ 1] * src[ 7] * src[14]
        - src[ 5] * src[ 2] * src[15]
        + src[ 5] * src[ 3] * src[14]
        + src[13] * src[ 2] * src[ 7]
        - src[13] * src[ 3] * src[ 6];

    dst[3] =
        - src[ 1] * src[ 6] * src[11]
        + src[ 1] * src[ 7] * src[10]
        + src[ 5] * src[ 2] * src[11]
        - src[ 5] * src[ 3] * src[10]
        - src[ 9] * src[ 2] * src[ 7]
        + src[ 9] * src[ 3] * src[ 6];

    dst[4] =
        - src[ 4] * src[10] * src[15]
        + src[ 4] * src[11] * src[14]
        + src[ 8] * src[ 6] * src[15]
        - src[ 8] * src[ 7] * src[14]
        - src[12] * src[ 6] * src[11]
        + src[12] * src[ 7] * src[10];

    dst[5] =
        + src[ 0] * src[10] * src[15]
        - src[ 0] * src[11] * src[14]
        - src[ 8] * src[ 2] * src[15]
        + src[ 8] * src[ 3] * src[14]
        + src[12] * src[ 2] * src[11]
        - src[12] * src[ 3] * src[10];

    dst[6] =
        - src[ 0] * src[ 6] * src[15]
        + src[ 0] * src[ 7] * src[14]
        + src[ 4] * src[ 2] * src[15]
        - src[ 4] * src[ 3] * src[14]
        - src[12] * src[ 2] * src[ 7]
        + src[12] * src[ 3] * src[ 6];

    dst[7] =
        + src[ 0] * src[ 6] * src[11]
        - src[ 0] * src[ 7] * src[10]
        - src[ 4] * src[ 2] * src[11]
        + src[ 4] * src[ 3] * src[10]
        + src[ 8] * src[ 2] * src[ 7]
        - src[ 8] * src[ 3] * src[ 6];

    dst[8] =
        + src[ 4] * src[ 9] * src[15]
        - src[ 4] * src[11] * src[13]
        - src[ 8] * src[ 5] * src[15]
        + src[ 8] * src[ 7] * src[13]
        + src[12] * src[ 5] * src[11]
        - src[12] * src[ 7] * src[ 9];

    dst[9] =
        - src[ 0] * src[ 9] * src[15]
        + src[ 0] * src[11] * src[13]
        + src[ 8] * src[ 1] * src[15]
        - src[ 8] * src[ 3] * src[13]
        - src[12] * src[ 1] * src[11]
        + src[12] * src[ 3] * src[ 9];

    dst[10] =
        + src[ 0] * src[ 5] * src[15]
        - src[ 0] * src[ 7] * src[13]
        - src[ 4] * src[ 1] * src[15]
        + src[ 4] * src[ 3] * src[13]
        + src[12] * src[ 1] * src[ 7]
        - src[12] * src[ 3] * src[ 5];

    dst[11] =
        - src[ 0] * src[ 5] * src[11]
        + src[ 0] * src[ 7] * src[ 9]
        + src[ 4] * src[ 1] * src[11]
        - src[ 4] * src[ 3] * src[ 9]
        - src[ 8] * src[ 1] * src[ 7]
        + src[ 8] * src[ 3] * src[ 5];

    dst[12] =
        - src[ 4] * src[ 9] * src[14]
        + src[ 4] * src[10] * src[13]
        + src[ 8] * src[ 5] * src[14]
        - src[ 8] * src[ 6] * src[13]
        - src[12] * src[ 5] * src[10]
        + src[12] * src[ 6] * src[ 9];

    dst[13] =
        + src[ 0] * src[ 9] * src[14]
        - src[ 0] * src[10] * src[13]
        - src[ 8] * src[ 1] * src[14]
        + src[ 8] * src[ 2] * src[13]
        + src[12] * src[ 1] * src[10]
        - src[12] * src[ 2] * src[ 9];

    dst[14] =
        - src[ 0] * src[ 5] * src[14]
        + src[ 0] * src[ 6] * src[13]
        + src[ 4] * src[ 1] * src[14]
        - src[ 4] * src[ 2] * src[13]
        - src[12] * src[ 1] * src[ 6]
        + src[12] * src[ 2] * src[ 5];

    dst[15] =
        + src[ 0] * src[ 5] * src[10]
        - src[ 0] * src[ 6] * src[ 9]
        - src[ 4] * src[ 1] * src[10]
        + src[ 4] * src[ 2] * src[ 9]
        + src[ 8] * src[ 1] * src[ 6]
        - src[ 8] * src[ 2] * src[ 5];

    /* Compute determinant: */

    det = + src[0] * dst[0] + src[1] * dst[4] + src[2] * dst[8] + src[3] * dst[12];

    /* Multiply adjoint with reciprocal of determinant: */

    det = 1.0f / det;

    dst[ 0] *= det;
    dst[ 1] *= det;
    dst[ 2] *= det;
    dst[ 3] *= det;
    dst[ 4] *= det;
    dst[ 5] *= det;
    dst[ 6] *= det;
    dst[ 7] *= det;
    dst[ 8] *= det;
    dst[ 9] *= det;
    dst[10] *= det;
    dst[11] *= det;
    dst[12] *= det;
    dst[13] *= det;
    dst[14] *= det;
    dst[15] *= det;
}
} // end anonymous namespace

mat4f inverse(mat4f a)
{
    // a,b should be either row-major or column-major
    assert(sizeof(a) == 16*sizeof(float));
    mat4f b;
    invert4x4(a[0], b[0]);
    return b;
}
