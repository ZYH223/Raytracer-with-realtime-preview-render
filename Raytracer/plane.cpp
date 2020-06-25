#include "plane.h"

Plane::Plane(Vec3f &n, float d, Material *m)
{
	normal = n;
	normal.Normalize();
	distance = d;
	material = m;
	// calculate the data for OpenGL
	Vec3f b1, b2;
	if (normal == Vec3f(1.0f, 0.0f, 0.0f)) Vec3f::Cross3(b1, normal, Vec3f(0.0f, 1.0f, 0.0f));
	else Vec3f::Cross3(b1, normal, Vec3f(1.0f, 0.0f, 0.0f));
	Vec3f::Cross3(b2, normal, b1);
	b1.Normalize(); b2.Normalize();
	size = 0xffff;// 65535 < 10^6
	Vec3f point[4] = { (normal * distance - size * b1 - size * b2),
		(normal * distance + size * b1 - size * b2),
		(normal * distance + size * b1 + size * b2),
		(normal * distance - size * b1 + size * b2)
	};
	for (int i = 0; i < 4; i++) 
	{
		p[i][0] = point[i].x();
		p[i][1] = point[i].y();
		p[i][2] = point[i].z();
	}
}

Plane::~Plane()
{
	//�ͷ��ڴ�Ĺ�������scene_parser����
	//delete material;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin, float tmax)
{
	bool paralleled = (fabs(r.getDirection().Dot3(normal)) < DIFF);
	if (!paralleled)
	{
		// �� p = dir*t+origin ���� p��n = d
		float t = (distance - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		if (tmin <= t && t <= tmax)
		{
			h.set(t, material, normal, r);
			return true;
		}
	}
	return false;
}

void Plane::paint(void)
{
	// shading
	material->glSetMaterial();
	// geometry
	glBegin(GL_QUADS);
	glNormal3f(normal.x(), normal.y(), normal.z());
	glVertex3fv(p[0]);
	glVertex3fv(p[1]);
	glVertex3fv(p[2]);
	glVertex3fv(p[3]);
	glEnd();
}