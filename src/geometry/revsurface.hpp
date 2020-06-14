#pragma once

#include "paramsurface.hpp"
#include "curve.hpp"

class RevSurface: public ParamSurface
{
private:
	Curve* curve;

	vec3f surface(float u, float v, vec3f& dpdu, vec3f& dpdv) const
	{
		vec3f p;
		auto pnd = curve->atParam(u);
		p = pnd.first;
		dpdu = pnd.second;
		// rotate around Y axis
	    float c = std::cos(-v*2*PI);
	    float s = std::sin(-v*2*PI);
		auto rot = mat4f(
	        c,  0, -s,  0,
	        0,  1,  0,  0,
	        s,  0,  c,  0,
	        0,  0,  0,  1
	    );
	    p = (rot * vec4f(p, 1)).xyz();
	    dpdu = (rot * vec4f(dpdu, 1)).xyz();
	    dpdv = cross(vec3f(p.x,0,p.z), vec3f(0,-2*PI,0));
	    return p;
	}
	
public:
	RevSurface(const Json& json)
	{
		auto curvetype = json["curve"]["type"];
		std::vector<vec3f> controls;
		for (auto p: json["curve"]["controls"])
			controls.push_back(json2vec3f(p));
		if (curvetype == "Bspline")
			curve = new BsplineCurve(controls);
		buildmesh();
	}
};


