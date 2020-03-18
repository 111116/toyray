// mathematics stuff
#pragma once

#include <cmath>
#include <iosfwd>


const float PI = acos(-1);


struct vec2f
{
	float x,y;
	vec2f(): x(0), y(0) {}
	vec2f(float p): x(p), y(p) {}
	vec2f(float px, float py): x(px), y(py) {}
	bool nonzero() const { return x || y; }
};

struct vec3f
{
	float x,y,z;
	vec3f(): x(0), y(0), z(0) {}
	vec3f(float p): x(p), y(p), z(p) {}
	vec3f(float px, float py, float pz): x(px), y(py), z(pz) {}
	bool nonzero() const { return x || y || z; }
};

struct vec4f
{
	float x,y,z,w;
	vec4f(): x(0), y(0), z(0), w(0) {}
	vec4f(float p): x(p), y(p), z(p), w(p) {}
	vec4f(float px, float py, float pz, float pw): x(px), y(py), z(pz), w(pw) {}
	vec4f(const vec3f& a, float pw): x(a.x), y(a.y), z(a.z), w(pw) {}
	vec3f xyz() const { return vec3f(x,y,z); }
	bool nonzero() const { return x || y || z || w; }
};


// IO operations
std::istream& operator >> (std::istream& is, vec2f& v);
std::istream& operator >> (std::istream& is, vec3f& v);
std::istream& operator >> (std::istream& is, vec4f& v);
std::ostream& operator << (std::ostream& os, const vec2f& v);
std::ostream& operator << (std::ostream& os, const vec3f& v);
std::ostream& operator << (std::ostream& os, const vec4f& v);


// single float operations
vec2f operator * (float k, const vec2f& v);
vec2f operator * (const vec2f& v, float k);
vec2f operator / (const vec2f& v, float k);

vec3f operator * (float k, const vec3f& v);
vec3f operator * (const vec3f& v, float k);
vec3f operator / (const vec3f& v, float k);

vec4f operator * (float k, const vec4f& v);
vec4f operator * (const vec4f& v, float k);
vec4f operator / (const vec4f& v, float k);

vec2f& operator *= (vec2f& a, float k);
vec3f& operator *= (vec3f& a, float k);
vec4f& operator *= (vec4f& a, float k);


// element-wise operations
vec2f operator + (const vec2f& a, const vec2f& b);
vec3f operator + (const vec3f& a, const vec3f& b);
vec4f operator + (const vec4f& a, const vec4f& b);
vec2f operator - (const vec2f& a, const vec2f& b);
vec3f operator - (const vec3f& a, const vec3f& b);
vec4f operator - (const vec4f& a, const vec4f& b);
vec2f operator * (const vec2f& a, const vec2f& b);
vec3f operator * (const vec3f& a, const vec3f& b);
vec4f operator * (const vec4f& a, const vec4f& b);

vec2f& operator += (vec2f& a, const vec2f& b);
vec3f& operator += (vec3f& a, const vec3f& b);
vec4f& operator += (vec4f& a, const vec4f& b);
vec2f& operator -= (vec2f& a, const vec2f& b);
vec3f& operator -= (vec3f& a, const vec3f& b);
vec4f& operator -= (vec4f& a, const vec4f& b);
vec2f& operator *= (vec2f& a, const vec2f& b);
vec3f& operator *= (vec3f& a, const vec3f& b);
vec4f& operator *= (vec4f& a, const vec4f& b);


// unary operations
vec2f operator-(const vec2f& v);
vec3f operator-(const vec3f& v);
vec4f operator-(const vec4f& v);

float sqrlen(const vec2f& v);
float sqrlen(const vec3f& v);
float sqrlen(const vec4f& v);

float norm(const vec2f& v);
float norm(const vec3f& v);
float norm(const vec4f& v);

vec2f normalized(const vec2f& v);
vec3f normalized(const vec3f& v);
vec4f normalized(const vec4f& v);


// other operations

float dot(const vec3f& a, const vec3f& b);

vec3f cross(const vec3f& a, const vec3f& b);

bool operator == (const vec3f& a, const vec3f& b);

