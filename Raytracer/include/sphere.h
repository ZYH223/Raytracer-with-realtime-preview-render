#pragma once
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "vectors.h"
#include "primitive.h"
#include <cmath>

class Sphere : public Primitive
{
public:
	Sphere(Vec3f c, float r, Material* m);
	~Sphere();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void insertIntoGrid(Grid* g, Matrix* m);
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
