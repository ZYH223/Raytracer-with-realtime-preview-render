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
	static void SetTessellationParameters(int theta_step, int phi_step, bool isgouraud);
	// data for OpenGL
	static float theta, phi;// data for tessellation
	static bool gouraud;
protected:
	Material* material;
	float radius;
	Vec3f center;
};

#endif // !SPHERE_H
