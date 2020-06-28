#include "Group.h"

Group::Group(int n)
{
	assert(n >= 0);
	list_size = n;
	list = new Object3D *[n];
	for (int i = 0; i < list_size; i++)
	{
		list[i] = nullptr;
	}
}

Group::~Group()
{
	for (int i = 0; i < list_size; i++)
	{
		delete[]list[i];
	}
	delete[]list;
}

void Group::addObject(int index, Object3D *obj)
{
	if (index >= list_size || index < 0)
	{
		std::cout << "Index: " << index << " is out of range[0," << (list_size - 1 > 0 ? list_size - 1 : 0) << "]" << std::endl;
		assert(0);
	}
	if (list[index] != nullptr)
	{
		delete[] list[index];
	}
	list[index] = obj;
}

bool Group::intersect(const Ray &r, Hit &h, float tmin, float tmax)
{
	bool hit_something = false;
	float current_t = tmax;
	for (int i = 0; i < list_size; i++)
	{
		if (list[i]->intersect(r, h, tmin, current_t))
		{
			current_t = h.getT();
			hit_something = true;
		}
	}
	return hit_something;
}

//bool Group::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)
//{
//	bool hit_something = false;
//	for (int i = 0; i < list_size; i++)
//	{
//		if (list[i]->intersectShadowRay(r, h, distanceToLight, color))// 命中
//		{
//			hit_something = true;
//			if (h.getMaterial()->getTransparentColor().Length() > EPSILON/*fabs(h.getT() - distanceToLight) > EPSILON*/)return hit_something;// 如果命中且遮挡，直接返回
//		}
//	}
//	return hit_something;
//}

bool Group::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)
{
	bool hit_something = false;
	//for (int i = 0; i < list_size; i++)
	//{
	//	if (list[i]->intersectShadowRay(r, h, distanceToLight, color))// 命中
	//	{
	//		hit_something = true;
	//		if (h.getMaterial()->getTransparentColor().Length() > EPSILON/*fabs(h.getT() - distanceToLight) > EPSILON*/)return hit_something;// 如果命中且遮挡，直接返回
	//	}
	//}
	Hit tempH(h);
	float current_t = EPSILON;
	while (intersect(r, tempH, current_t, distanceToLight))
	{
		hit_something = true;
		if (tempH.getMaterial()->getTransparentColor().Length() < EPSILON) 
		{
			h.set(tempH.getT(), tempH.getMaterial(), tempH.getNormal(), r);
			break;
		}
		if (tempH.getNormal().Dot3(r.getDirection()) > 0)
		{
			color = 0.5f / (tempH.getT() - h.getT()) * color * tempH.getMaterial()->getTransparentColor();
		}
		h.set(tempH.getT(), tempH.getMaterial(), tempH.getNormal(), r);
		current_t = tempH.getT()+EPSILON;
	}
	return hit_something;
}

void Group::paint(void) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->paint();
	}
}