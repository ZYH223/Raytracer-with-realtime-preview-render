#pragma once
#include "object3d.h"
#include "grid.h"

class Primitive : public Object3D
{
public:
	virtual bool intersect(const Ray& r, Hit& h, float tmin, float tmax) = 0;
	//virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color);
	virtual void paint(void) = 0;
};