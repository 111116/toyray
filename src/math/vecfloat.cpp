
#include <cmath>
#include <cstring>
#include <iostream>


struct vec2f;
struct vec3f;
struct vec4f;

const float PI = acos(-1);


// basic definitions

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

std::istream& operator >> (std::istream& is, vec2f& v) {
	return is >> v.x >> v.y;
}
std::istream& operator >> (std::istream& is, vec3f& v) {
	return is >> v.x >> v.y >> v.z;
}
std::istream& operator >> (std::istream& is, vec4f& v) {
	return is >> v.x >> v.y >> v.z >> v.w;
}
std::ostream& operator << (std::ostream& os, const vec2f& v) {
	return os << '(' << v.x << ',' << v.y << ')';
}
std::ostream& operator << (std::ostream& os, const vec3f& v) {
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}
std::ostream& operator << (std::ostream& os, const vec4f& v) {
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
}



// single float operations

vec2f operator * (float k, const vec2f& v) {
	return vec2f(k*v.x, k*v.y);
}
vec2f operator * (const vec2f& v, float k) {
	return vec2f(k*v.x, k*v.y);
}
vec2f operator / (const vec2f& v, float k) {
	return vec2f(v.x/k, v.y/k);
}

vec3f operator * (float k, const vec3f& v) {
	return vec3f(k*v.x, k*v.y, k*v.z);
}
vec3f operator * (const vec3f& v, float k) {
	return vec3f(k*v.x, k*v.y, k*v.z);
}
vec3f operator / (const vec3f& v, float k) {
	return vec3f(v.x/k, v.y/k, v.z/k);
}

vec4f operator * (float k, const vec4f& v) {
	return vec4f(k*v.x, k*v.y, k*v.z, k*v.w);
}
vec4f operator * (const vec4f& v, float k) {
	return vec4f(k*v.x, k*v.y, k*v.z, k*v.w);
}
vec4f operator / (const vec4f& v, float k) {
	return vec4f(v.x/k, v.y/k, v.z/k, v.w/k);
}

vec2f& operator *= (vec2f& a, float k)
{
	a.x *= k;
	a.y *= k;
	return a;
}
vec3f& operator *= (vec3f& a, float k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
}
vec4f& operator *= (vec4f& a, float k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	a.w *= k;
	return a;
}



// element-wise operations

vec2f operator + (const vec2f& a, const vec2f& b) {
	return vec2f(a.x + b.x, a.y + b.y);
}
vec3f operator + (const vec3f& a, const vec3f& b) {
	return vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec4f operator + (const vec4f& a, const vec4f& b) {
	return vec4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
vec2f operator - (const vec2f& a, const vec2f& b) {
	return vec2f(a.x - b.x, a.y - b.y);
}
vec3f operator - (const vec3f& a, const vec3f& b) {
	return vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}
vec4f operator - (const vec4f& a, const vec4f& b) {
	return vec4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
vec2f operator * (const vec2f& a, const vec2f& b) {
	return vec2f(a.x*b.x, a.y*b.y);
}
vec3f operator * (const vec3f& a, const vec3f& b) {
	return vec3f(a.x*b.x, a.y*b.y, a.z*b.z);
}
vec4f operator * (const vec4f& a, const vec4f& b) {
	return vec4f(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

vec2f& operator += (vec2f& a, const vec2f& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
vec3f& operator += (vec3f& a, const vec3f& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
vec4f& operator += (vec4f& a, const vec4f& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

vec2f& operator -= (vec2f& a, const vec2f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
vec3f& operator -= (vec3f& a, const vec3f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
vec4f& operator -= (vec4f& a, const vec4f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

vec2f& operator *= (vec2f& a, const vec2f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
vec3f& operator *= (vec3f& a, const vec3f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
vec4f& operator *= (vec4f& a, const vec4f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}



// unary operations
vec2f operator-(const vec2f& v) {
	return vec2f(-v.x, -v.y);
}
vec3f operator-(const vec3f& v) {
	return vec3f(-v.x, -v.y, -v.z);
}
vec4f operator-(const vec4f& v) {
	return vec4f(-v.x, -v.y, -v.z, -v.w);
}
float sqrlen(const vec2f& v) {
	return v.x*v.x + v.y*v.y;
}
float sqrlen(const vec3f& v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
float sqrlen(const vec4f& v) {
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}
float norm(const vec2f& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}
float norm(const vec3f& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
float norm(const vec4f& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
vec2f normalized(const vec2f& v)
{
	float lambda = 1/norm(v);
	return vec2f(lambda*v.x, lambda*v.y);
}
vec3f normalized(const vec3f& v)
{
	float lambda = 1/norm(v);
	return vec3f(lambda*v.x, lambda*v.y, lambda*v.z);
}
vec4f normalized(const vec4f& v)
{
	float lambda = 1/norm(v);
	return vec4f(lambda*v.x, lambda*v.y, lambda*v.z, lambda*v.w);
}



// other operations

float dot(const vec3f& a, const vec3f& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3f cross(const vec3f& a, const vec3f& b)
{
	return vec3f(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

bool operator == (const vec3f& a, const vec3f& b) {
	return a.x==b.x && a.y==b.y && a.z==b.z;
}