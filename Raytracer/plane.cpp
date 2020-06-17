#include "plane.h"

Plane::Plane(Vec3f &n, float d, Material *m)
{
	normal = n;
	normal.Normalize();
	distance = d;
	material = m;
}

Plane::~Plane()
{
	delete material;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin, float tmax)
{
	bool paralleled = (fabs(r.getDirection().Dot3(normal)) < DIFF);
	if (!paralleled)
	{
		// ½« p = dir*t+origin ´úÈë p¡¤n = d
		float t = (distance - normal.Dot3(r.getOrigin())) / (normal.Dot3(r.getDirection()));
		if (tmin <= t && t <= tmax)
		{
			h.set(t, material, normal, r);
			return true;
		}
	}
	return false;
}