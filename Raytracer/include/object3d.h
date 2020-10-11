#pragma once
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "constant.h"
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "boundingbox.h"
//#include "grid.h"

class Grid;

class Object3D
{
public:
	BoundingBox* getBoundingBox() { return bb; }
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax) = 0;
	//virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color) = 0;
	virtual void paint(void) = 0;
	virtual void insertIntoGrid(Grid* g, Matrix* m) {}
protected:
	BoundingBox* bb = nullptr;
};
#endif // !OBJECT3D_H_