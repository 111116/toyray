#pragma once

struct PinholeCamera : public Camera
{
	vec3f ray_central, uphalf, lefthalf;

	PinholeCamera(const Json& conf): Camera(conf) {
		ray_central = look_at - position;
		uphalf = norm(ray_central) * resy / resx * tan(fov/2) * up;
		lefthalf = cross(up, ray_central) * tan(fov/2);
	}
	
	Ray sampleray(vec2f pos) const {
		return Ray(position, normalized(ray_central + (1-pos.x*2) * lefthalf + (1-pos.y*2) * uphalf));
	}
};
