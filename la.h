#ifndef LA_H
#define LA_H

#include <cmath>
#include <cstring>



struct vec3
{
	float x,y,z;

	vec3(): x(0), y(0), z(0) {}
	vec3(float px, float py, float pz): x(px), y(py), z(pz) {}
};


vec3& operator += (vec3& a, vec3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
vec3& operator -= (vec3& a, vec3 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
vec3 operator + (vec3 a, vec3 b)
{
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec3 operator - (vec3 a, vec3 b)
{
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
vec3 operator - (vec3 b)
{
	return vec3(-b.x, -b.y, -b.z);
}

vec3& operator *= (vec3& a, float k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
}
vec3 operator * (float k, vec3 v)
{
	return vec3(k*v.x, k*v.y, k*v.z);
}
vec3 operator * (vec3 v, float k)
{
	return vec3(k*v.x, k*v.y, k*v.z);
}


float dot(vec3 a, vec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 cross(vec3 a, vec3 b)
{
	return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

float norm(vec3 v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
vec3 normalize(vec3 v)
{
	float lambda = 1/norm(v);
	return vec3(lambda*v.x, lambda*v.y, lambda*v.z);
}


struct mat3
{
	float data[3][3];
	mat3()
	{
		memset(data, 0, sizeof data);
	}

	mat3(vec3 a, vec3 b, vec3 c)
	{
		data[0][0] = a.x;
		data[1][0] = a.y;
		data[2][0] = a.z;
		data[0][1] = b.x;
		data[1][1] = b.y;
		data[2][1] = b.z;
		data[0][2] = c.x;
		data[1][2] = c.y;
		data[2][2] = c.z;
	}

	float* operator[](unsigned i)
	{
		return data[i];
	}
};

mat3 diag(int x, int y, int z)
{
	mat3 res;
	res[0][0] = x;
	res[1][1] = y;
	res[2][2] = z;
	return res;
}

mat3& operator += (mat3& a, mat3 b)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] += b[i][j];
	return a;
}

mat3 operator + (mat3 a, mat3 b)
{
	mat3 res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = a[i][j] + b[i][j];
	return res;
}

mat3& operator -= (mat3& a, mat3 b)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] -= b[i][j];
	return a;
}

mat3 operator - (mat3 a, mat3 b)
{
	mat3 res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = a[i][j] - b[i][j];
	return res;
}

mat3& operator *= (mat3& a, float k)
{
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			a[i][j] *= k;
	return a;
}

mat3 operator * (float k, mat3 b)
{
	mat3 res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[i][j] = k * b[i][j];
	return res;
}

mat3 operator * (mat3 a, mat3 b)
{
	mat3 res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			for (int k=0; k<3; ++k)
				res[i][j] += a[i][k] * b[k][j];
	return res;
}

vec3 operator * (mat3 a, vec3 b)
{
	return vec3(
		a[0][0] * b.x + a[0][1] * b.y + a[0][2] * b.z,
		a[1][0] * b.x + a[1][1] * b.y + a[1][2] * b.z,
		a[2][0] * b.x + a[2][1] * b.y + a[2][2] * b.z
	);
}

float det(mat3 a)
{
	return
		+ a[0][0] * a[1][1] * a[2][2]
		+ a[0][1] * a[1][2] * a[2][0]
		+ a[0][2] * a[1][0] * a[2][1]
		- a[0][0] * a[1][2] * a[2][1]
		- a[0][1] * a[1][0] * a[2][2]
		- a[0][2] * a[1][1] * a[2][0];
}

mat3 inverse(mat3 a)
{
	float invdet = 1/det(a);
	mat3 res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			res[j][i] = (a[(i+1)%3][(j+1)%3] * a[(i+2)%3][(j+2)%3] - a[(i+2)%3][(j+1)%3] * a[(i+1)%3][(j+2)%3]) * invdet;
	return res;	
}
#endif
