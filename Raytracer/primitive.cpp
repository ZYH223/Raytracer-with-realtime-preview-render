#include "object3d.h"

//bool Primitive::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// ���������δ���з���false���������赲����true�����е����赲��͸��������true��t�Ա���distanceToLight����
//{ 
//	bool hit_something = false;
//	Hit tempH(distanceToLight, nullptr, Vec3f());
//	if (intersect(r, tempH, EPSILON, distanceToLight))
//	{
//		hit_something = true;// ���ڵ�
//		if (tempH.getMaterial()->getTransparentColor().Length() < EPSILON) return hit_something;// ����ȫ�ڵ�
//		//if (h.getMaterial() != nullptr && r.getDirection().Dot3(tempH.getNormal())* r.getDirection().Dot3(h.getNormal()) < 0)// ��Ϊ�ȼ��������滹�ǳ������ǲ�ȷ���ģ�����Ҫ�ж����֮ǰ�Ѿ�����ײ����ǰһ����ײ����һ����ײ�ĳ�������/���䣩��ͬ
//		//{
//		//	float dis = tempH.getT() - h.getT();// ����ʹ�õ�t�п��ܱ�transform����
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