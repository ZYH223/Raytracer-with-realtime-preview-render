#pragma once
#include "object3d.h"
#include "primitive.h"

class Plane : public Primitive {
public:
	Plane(Vec3f &n, float d, Material *m);
	~Plane();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void paint(void);
private:
	Material* material;
	Vec3f normal;
	float distance;
	// data for OpenGL
	float size;
	GLfloat p[4][3];
};