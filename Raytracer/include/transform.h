#pragma once
#include "object3d.h"
#include "matrix.h"

class Transform : public Object3D {
public:
	Transform(Matrix& m, Object3D* o);
	~Transform();
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color);
	virtual void paint(void);
private:
	Object3D* object;
	Matrix matrix, inversedMatrix, transposedInversedMatrix;
	GLfloat *glMatrix;
};