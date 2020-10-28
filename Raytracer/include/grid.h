#pragma once
#include "object3d.h"
#include "object3dvector.h"
#include "boundingbox.h"
#include "marchingInfo.h"

class Grid : public Object3D 
{
public:
	Grid(BoundingBox* bb, int nx, int ny, int nz);
	~Grid();
	int CellNumX() { return nx; }
	int CellNumY() { return ny; }
	int CellNumZ() { return nz; }
	Vec3f GetLength() const;
	Object3DVector GetCell(int x, int y, int z);
	void AddToCell(int x, int y, int z, Object3D *object);
	Vec3f GetCoordinate(int x, int y, int z);

	void insertBoundingBox(BoundingBox *target, Object3D *object);
	void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;
	virtual bool intersect(const Ray &r, Hit &h, float tmin, float tmax);
	virtual void paint(void);
	void print(void);
protected:
	Object3DVector*** cells;
	int nx, ny, nz;
	Material* material;
	Material** debugMaterials;
};