#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <float.h>
#include "constant.h"
#include "ray.h"

class Camera {
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
};

class OrthographicCamera : public Camera {
private:
	float tmin;
	Vec3f center;
	float size;
	Vec3f direction, horizontal, up;
public:
	OrthographicCamera(Vec3f c, Vec3f d, Vec3f u, float s);
	float getTMin() const { return tmin; }
	Ray generateRay(Vec2f point);
};

class PerspectiveCamera : public Camera {
private:
	float tmin;
	Vec3f center;
	float fov;
	Vec3f direction, horizontal, up;
public:
	PerspectiveCamera(Vec3f c, Vec3f d, Vec3f u, float a);
	float getTMin() const { return tmin; }
	Ray generateRay(Vec2f point);
};

#endif // !CAMERA_H