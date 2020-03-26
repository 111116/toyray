#pragma once

struct PinholeCamera : public Camera
{
	vec3f dir, uphalf, lefthalf;

	PinholeCamera(const Json& conf): Camera(conf) {
		dir = normalized(look_at - position);
		up = normalized(cross(dir, cross(up, dir)));
		uphalf = norm(dir) * resy / resx * tan(fov/2) * up;
		lefthalf = cross(up, dir) * tan(fov/2);
		if (mirror) lefthalf = -lefthalf;
	}
	
	Ray sampleray(vec2f pos) const {
		return Ray(position, normalized(dir + (1-pos.x*2) * lefthalf + (1-pos.y*2) * uphalf));
	}
};
