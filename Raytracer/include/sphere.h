#pragma once
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "vectors.h"
#include "Object3D.h"
#include <cmath>

class Sphere : public Object3D
{
public:
	Sphere(Vec3f c, float r, Material* m);
	~Sphere();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void paint(void);
	void SetTessellationParameters(float theta, float phi, bool gouraud);
protected:
	Material* material;
	float radius;
	Vec3f center;
	// data for OpenGL
	float theta, phi;// data for tessellation
	bool gouraud;
};

#endif // !SPHERE_H
