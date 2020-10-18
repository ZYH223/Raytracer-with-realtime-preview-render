#pragma once
#include "object3d.h"
#include "boundingbox.h"

class Grid : public Object3D 
{
public:
	Grid(BoundingBox* bb, int nx, int ny, int nz);
	~Grid();
	virtual bool intersect(const Ray& r, Hit& h, float tmin, float tmax);
	virtual bool intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color);
	virtual void paint(void);
	void print(void);
	int CellNumX() { return nx; }
	int CellNumY() { return ny; }
	int CellNumZ() { return nz; }
	Vec3f GetLength();
	bool GetCell(int x, int y, int z);
	void SetCell(int x, int y, int z, bool opaque);
	Vec3f GetCoordinate(int x, int y, int z);
protected:
	bool*** cells;
	int nx, ny, nz;
};