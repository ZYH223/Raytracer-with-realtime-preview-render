#pragma once
#include "constant.h"
#include "vectors.h"

class MarchingInfo {
public:

private:
	int tmin;
	int grid_i, grid_j, grid_k;
	Vec3f t_next;
	Vec3f increments;
	Vec3f sign;
};