#pragma once
#include "constant.h"
#include "vectors.h"

class MarchingInfo {
public:
	// Initialize and accessor
	void init(Vec3f length, Vec3f dir, float t) { 
		increments = Vec3f(fabs(length.x() / dir.x()), fabs(length.y() / dir.y()), fabs(length.z() / dir.z()));
		sign = Vec3f(dir.x() > 0.0f ? 1.0f : -1.0f, dir.y() > 0.0f ? 1.0f : -1.0f, dir.z() > 0.0f ? 1.0f : -1.0f);
		tmin = t;
	}
	void setOrigin(float t1, float t2, int i, int j, int k, Vec3f next) {// next_x代表从起始点到x轴上下一格还需要的t
		tmin = t1; tmax = t2;
		grid_i = i, grid_j = j, grid_k = k;
		t_next = Vec3f(tmin + next.x(), tmin + next.y(), tmin + next.z());
		//lastThrough = 0;
		std::cout << grid_i << ' ' << grid_j << ' ' << grid_k << std::endl;
	}
	void setTMin(float t) { tmin = t; }
	float getTMin() { return tmin; }
	void setIndecies(int i, int j, int k) { grid_i = i, grid_j = j, grid_k = k; }
	int getI() { return grid_i; }
	int getJ() { return grid_j; }
	int getK() { return grid_k; }
	Vec3f getSign() { return sign; }
	Vec3f getIncrements() { return increments; }

	// Functional
	void nextCell() {
		if (tmax - tmin > EPSILON) {
			// 找到min(t_next_x, t_next_y, t_next_z)作为nextCell
			if (t_next.y() - t_next.x() > EPSILON && t_next.z() - t_next.x() > EPSILON) {
				grid_i += (int)sign.x();
				tmin = t_next.x();
				t_next.Set(t_next.x() + increments.x(), t_next.y(), t_next.z());
			}
			else if (t_next.x() - t_next.y() > EPSILON && t_next.z() - t_next.y() > EPSILON) {
				grid_j += (int)sign.y();
				tmin = t_next.y();
				t_next.Set(t_next.x(), t_next.y() + increments.y(), t_next.z());
			}
			else {
				grid_k += (int)sign.z();
				tmin = t_next.z();
				t_next.Set(t_next.x(), t_next.y(), t_next.z() + increments.z());
			}
			std::cout << grid_i << ' ' << grid_j << ' ' << grid_k << std::endl;
		}
	}
	int getNextThrough() {
		int nextThrough = 0;
		if (tmax - tmin > EPSILON) {
			// 找到min(t_next_x, t_next_y, t_next_z)
			if (t_next.y() - t_next.x() > EPSILON && t_next.z() - t_next.x() > EPSILON) nextThrough = sign.x() > 0 ? 1 : 2;// 沿x轴正向则穿过当前Cell的右面左面，反向则左面
			else if (t_next.x() - t_next.y() > EPSILON && t_next.z() - t_next.y() > EPSILON) nextThrough = sign.y() > 0 ? 3 : 4;// 沿y轴正向则穿过当前Cell的顶面，反向则底面
			else nextThrough = sign.z() > 0 ? 5 : 6;// 沿z轴正向则穿过当前Cell的正面，反向则背面
		}
		return nextThrough;
	}
	bool end() { return tmax - tmin < EPSILON; }
private:
	float tmin = EPSILON, tmax = FLT_MAX;
	int grid_i = 0, grid_j = 0, grid_k = 0;
	Vec3f t_next;
	Vec3f increments;
	Vec3f sign;
	//int lastThrough = 0;
};