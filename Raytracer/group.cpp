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
	//bb = new BoundingBox(Vec3f(), Vec3f()); 不能初始化为(0,0,0)
	/*if (list_size > 0) {
		bb = new BoundingBox(list[0]->getBoundingBox()->getMin(), list[0]->getBoundingBox()->getMax());
		for (int i = 1; i < list_size; i++) {
			if(list[i]->getBoundingBox() != nullptr) bb->Extend(list[i]->getBoundingBox());
		}
	}
	else bb = nullptr;*/
	bb = nullptr;
}

Group::~Group()
{
	for (int i = 0; i < list_size; i++)
	{
		delete[]list[i];
	}
	delete[]list;
	delete bb;
}

void Group::addObject(int index, Object3D *obj)
{
	if (index >= list_size || index < 0)
	{
		std::cout << "[ERROR](Group::addObject)Index: " << index << " is out of range[0," << (list_size - 1 > 0 ? list_size - 1 : 0) << "]" << std::endl;
		assert(0);
	}
	if (list[index] != nullptr)
	{
		delete[] list[index];
	}
	list[index] = obj;
	if (bb == nullptr) {
		bb = new BoundingBox(obj->getBoundingBox()->getMin(), obj->getBoundingBox()->getMax());// 为了防止奇异情况，用第一个图元的bb作为起始
	}
	else if (obj->getBoundingBox() != nullptr) {// 防止包含平面、空group的bb（他们的bb都是nullptr）
		bb->Extend(obj->getBoundingBox());
	}
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

void Group::insertIntoGrid(Grid* g, Matrix* m) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->insertIntoGrid(g, m);
	}
}

void Group::paint(void) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->paint();
	}
}