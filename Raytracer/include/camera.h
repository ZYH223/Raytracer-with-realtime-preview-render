#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <float.h>
#include "constant.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ray.h"

class Camera {
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;

	virtual void glInit(int w, int h) = 0;
	virtual void glPlaceCamera(void) = 0;
	virtual void dollyCamera(float dist) = 0;
	virtual void truckCamera(float dx, float dy) = 0;
	virtual void rotateCamera(float rx, float ry) = 0;
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
	void glInit(int w, int h);
	void glPlaceCamera(void);
	void dollyCamera(float dist);
	void truckCamera(float dx, float dy);
	void rotateCamera(float rx, float ry);
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
	void glInit(int w, int h);
	void glPlaceCamera(void);
	void dollyCamera(float dist);
	void truckCamera(float dx, float dy);
	void rotateCamera(float rx, float ry);
};

#endif // !CAMERA_H