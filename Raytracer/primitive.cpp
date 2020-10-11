#include "object3d.h"

//bool Primitive::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// 三种情况：未命中返回false，命中且阻挡返回true，命中但不阻挡（透明）返回true但t仍保持distanceToLight不变
//{ 
//	bool hit_something = false;
//	Hit tempH(distanceToLight, nullptr, Vec3f());
//	if (intersect(r, tempH, EPSILON, distanceToLight))
//	{
//		hit_something = true;// 被遮挡
//		if (tempH.getMaterial()->getTransparentColor().Length() < EPSILON) return hit_something;// 被完全遮挡
//		//if (h.getMaterial() != nullptr && r.getDirection().Dot3(tempH.getNormal())* r.getDirection().Dot3(h.getNormal()) < 0)// 因为先计算入射面还是出射面是不确定的，所以要判断如果之前已经有碰撞而且前一次碰撞和这一次碰撞的朝向（入射/出射）不同
//		//{
//		//	float dis = tempH.getT() - h.getT();// 这里使用的t有可能被transform更改
//		//	color = dis * color * h.getMaterial()->getTransparentColor();
//		//}
//		if (r.getDirection().Dot3(tempH.getNormal()) < 0)
//		{
//			color = color * tempH.getMaterial()->getTransparentColor();
//		}
//		h.set(tempH.getT(), tempH.getMaterial(), tempH.getNormal(), r);
//		
//	}
//	return hit_something;
//}