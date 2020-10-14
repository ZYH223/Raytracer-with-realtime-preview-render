#pragma once
#include "primitive.h"

class Triangle : public Primitive {
public:
	Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
	~Triangle();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void paint(void);
	virtual void insertIntoGrid(Grid* g, Matrix* m);
protected:
	Material* material;
	Vec3f a, b, c;
};