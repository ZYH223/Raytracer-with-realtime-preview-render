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

void Group::paint(void) {
	for (int i = 0; i < list_size; i++)
	{
		list[i]->paint();
	}
}