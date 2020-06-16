#pragma once

struct ThinlensCamera : public Camera
{
	vec3f dir, uphalf, lefthalf;
	float focaldistance;
	float lensradius;

	ThinlensCamera(const Json& conf): Camera(conf) {
		focaldistance = conf["focaldistance"];
		lensradius = conf["lensradius"];
		dir = normalized(look_at - position);
		up = normalized(cross(dir, cross(up, dir)));
		uphalf = norm(dir) * resy / resx * tan(fov/2) * up;
		lefthalf = cross(up, dir) * tan(fov/2);
		if (mirror) lefthalf = -lefthalf;
	}
	
	Ray sampleray(vec2f pos, Sampler& sampler) const {
		vec2f u = sampler.sampleUnitDisk();
		vec3f aperture_sample = lensradius * (u.x * normalized(lefthalf) + u.y * normalized(uphalf));
		vec3f focused_rel = (dir + (1-pos.x*2) * lefthalf + (1-pos.y*2) * uphalf) * focaldistance;
		return Ray(position + aperture_sample, normalized(focused_rel - aperture_sample));
	}
};
