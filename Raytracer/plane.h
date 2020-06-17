#pragma once
#include "object3d.h"
class Plane : public Object3D {
public:
	Vec3f normal;
	float distance;
	Material* material;
	Plane(Vec3f &n, float d, Material *m);
	~Plane();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
};