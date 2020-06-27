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

bool Group::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)
{
	//bool hit_something = false;
	for (int i = 0; i < list_size; i++)
	{
		if (list[i]->intersectShadowRay(r, h, distanceToLight, color))
		{
			//hit_something = true;// 被遮挡
			if(h.getMaterial()->getTransparentrColor().Length() < EPSILON) return true;// 被完全遮挡
			if (r.getDirection().Dot3(h.getNormal()) < 0)// 仅入射时计算透明阴影
				color = color * h.getMaterial()->getTransparentrColor();
				//color = color * 0.5f;
			h.set(distanceToLight, h.getMaterial(), h.getNormal(), r);
		}
	}
	return false;// 未被遮挡或者被透明材质遮挡
}

void Group::paint(void) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->paint();
	}
}