#include "object3d.h"

bool Primitive::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// 三种情况：未命中返回false，命中且阻挡返回true，命中但不阻挡（透明）返回true但t仍保持distanceToLight不变
{ 
	bool hit_something = false;
		if (intersect(r, h, EPSILON, distanceToLight))
		{
			hit_something = true;// 被遮挡
			if (h.getMaterial()->getTransparentrColor().Length() < EPSILON) return hit_something;// 被完全遮挡
			if (r.getDirection().Dot3(h.getNormal()) < 0)// 仅入射时计算透明阴影
				color = color * h.getMaterial()->getTransparentrColor();
			h.set(distanceToLight, h.getMaterial(), h.getNormal(), r);
		}
	return hit_something;
}