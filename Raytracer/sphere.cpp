#include "sphere.h"

Sphere::Sphere(Vec3f c, float r, Material* m)
{
	radius = r;
	center = c;
	material =  new Material(*m);
}

Sphere::~Sphere()
{
	delete material;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin, float tmax)
{
#pragma region geometry_function

	 Vec3f distance = center - r.getOrigin();
	 Vec3f dir = r.getDirection();// dir用于计算距离，r.dir不一定是归一化的
	 dir.Normalize();
	 float t0 = distance.Dot3(dir);
	 float tmp = distance.Length() * distance.Length() - t0 * t0;
	 if (tmp > radius * radius)return false;// 不相交
	 float t = sqrt(radius * radius - tmp);
	 if (/*t0 - t < h.getT() && */tmin <= t0 - t && t0 - t <= tmax)
	 {
		 //cout << material->getDiffuseColor() << endl;
		 Vec3f n = r.pointAtParameter(t0 - t) - center;
		 n.Normalize();
		 h.set(t0 - t, material, n, r);
		 return true;
	 }
	 if (/*t0 + t < h.getT() && */tmin <= t0 + t && t0 + t <= tmax)
	 {
		 //cout << material->getDiffuseColor() << endl;
		 Vec3f n = r.pointAtParameter(t0 + t) - center;
		 n.Normalize();
		 h.set(t0 + t, material, n, r);
		 return true;
	 }
	 return false;

#pragma endregion
#pragma region algebra_function
	 //float a = r.getDirection().Dot3(r.getDirection());
	 //float b = 2 * r.getDirection().Dot3(r.getOrigin() - center);
	 //float c = (r.getOrigin() - center).Dot3(r.getOrigin() - center) - radius * radius;
	 //if (b*b - 4 * a*c < 0)
	 //{
	 //	return false;
	 //}
	 //else
	 //{
	 //	float tmp = sqrt(b*b - 4 * a*c);
	 //	float t1 = (-b - tmp) / (2 * a);// 先取离得近的根
	 //	if (/*t1 < h.getT() && */tmin <= t1 && t1 <= tmax)
	 //	{
	 //		h.set(t1, material, r);
	 //		return true;
	 //	}
	 //	float t2 = (-b + tmp) / (2 * a);// 再取离得远的根
	 //	if (/*t2 < h.getT() && */tmin <= t2 && t2 <= tmax)
	 //	{
	 //		h.set(t2, material, r);
	 //		return true;
	 //	}
	 //}
	 //return false;
#pragma endregion

	}