#include "object3d.h"

bool Primitive::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// ���������δ���з���false���������赲����true�����е����赲��͸��������true��t�Ա���distanceToLight����
{ 
	bool hit_something = false;
		if (intersect(r, h, EPSILON, distanceToLight))
		{
			hit_something = true;// ���ڵ�
			if (h.getMaterial()->getTransparentrColor().Length() < EPSILON) return hit_something;// ����ȫ�ڵ�
			if (r.getDirection().Dot3(h.getNormal()) < 0)// ������ʱ����͸����Ӱ
				color = color * h.getMaterial()->getTransparentrColor();
			h.set(distanceToLight, h.getMaterial(), h.getNormal(), r);
		}
	return hit_something;
}