#include "triangle.h"

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m)
{
	this->a = a;
	this->b = b;
	this->c = c;
	material = m;
}

Triangle::~Triangle()
{
	//释放内存的工作交给scene_parser来做
	//delete material;
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin, float tmax)// 先与平面求交
{
	Vec3f u = b - a, v = c - b, w = a - c, normal;
	Vec3f::Cross3(normal, u, v);
	normal.Normalize();
	if (fabs(normal.Dot3(r.getDirection())) < DIFF)return false;// 平行
	float distance = a.Dot3(normal);
	float t = (distance - r.getOrigin().Dot3(normal)) / (r.getDirection().Dot3(normal));
	if (tmin <= t && t <= tmax)
	{
		Vec3f p = r.pointAtParameter(t);// 求出交点
		Vec3f pa = p - a, pb = p - b, pc = p - c, npab, npbc, npca;// 计算是否在三角形内
		Vec3f::Cross3(npab, pa, pb); Vec3f::Cross3(npbc, pb, pc); Vec3f::Cross3(npca, pc, pa);
		bool dir_ab_bc = npab.Dot3(npbc) >= 0, dir_bc_ca = npbc.Dot3(npca) >= 0, dir_ca_ab = npca.Dot3(npab) >= 0;
		if (dir_ab_bc && dir_bc_ca && dir_ca_ab)// 在三角形内
		{
			h.set(t, material, normal, r);
			return true;
		}
	}
	return false;
}

void Triangle::paint(void) 
{
	// shading
	material->glSetMaterial();
	// geometry
	glBegin(GL_TRIANGLES);
	glVertex3f(a.x(), a.y(), a.z());
	glVertex3f(b.x(), b.y(), b.z());
	glVertex3f(c.x(), c.y(), c.z());
	Vec3f normal;
	Vec3f::Cross3(normal, b - a, c - b);
	glNormal3f(normal.x(), normal.y(), normal.z());
	glEnd();
}