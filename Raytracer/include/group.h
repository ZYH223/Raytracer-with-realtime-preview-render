#pragma once
#ifndef _GROUP_H_
#define _GROUP_H_

#include "vectors.h"
#include "Object3D.h"

class Group : public Object3D
{
public:
	Group() {}
	~Group();
	Group(int n);
	void addObject(int index, Object3D *obj);
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void paint(void);
protected:
	int list_size;
	Object3D **list;
};
#endif // !GROUP_H
