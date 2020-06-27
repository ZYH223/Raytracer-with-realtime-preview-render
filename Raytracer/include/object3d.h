#pragma once
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "constant.h"
#include "ray.h"
#include "hit.h"
#include "material.h"

class Object3D
{
public:
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax) = 0;
	virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color) = 0;
	virtual void paint(void) = 0;
};

class Primitive: public Object3D
{
public:
	virtual bool intersect(const Ray& r, Hit& h, float tmin, float tmax) = 0;
	virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color) { return intersect(r, h, EPSILON, distanceToLight); }
	virtual void paint(void) = 0;
};

#endif // !OBJECT3D_H