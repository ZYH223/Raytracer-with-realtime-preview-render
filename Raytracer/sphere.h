#pragma once
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "vectors.h"
#include "Object3D.h"
#include <cmath>

class Sphere : public Object3D
{
public:
	float radius;
	Vec3f center;
	Material* material;
	Sphere(Vec3f c, float r, Material* m);
	~Sphere();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
};

#endif // !SPHERE_H
