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
			//hit_something = true;// ���ڵ�
			if(h.getMaterial()->getTransparentrColor().Length() < EPSILON) return true;// ����ȫ�ڵ�
			if (r.getDirection().Dot3(h.getNormal()) < 0)// ������ʱ����͸����Ӱ
				color = color * h.getMaterial()->getTransparentrColor();
				//color = color * 0.5f;
			h.set(distanceToLight, h.getMaterial(), h.getNormal(), r);
		}
	}
	return false;// δ���ڵ����߱�͸�������ڵ�
}

void Group::paint(void) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->paint();
	}
}