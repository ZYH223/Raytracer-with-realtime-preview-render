#pragma once
#include "object3d.h"

class Triangle : public Object3D {
public:
	Vec3f a, b, c;
	Material* material;
	Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
	~Triangle();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
};