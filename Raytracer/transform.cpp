#include "transform.h"

Transform::Transform(Matrix& m, Object3D* o)
{
	matrix = m;
	object = o; 
	inversedMatrix = matrix;
	inversedMatrix.Inverse(EPSILON);
	transposedInversedMatrix = inversedMatrix;
	transposedInversedMatrix.Transpose();

	// calculate data for OpenGL
	glMatrix = matrix.glGet();
}

Transform::~Transform()
{
	delete[] glMatrix;
	delete object;
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin, float tmax)
{
	// 变换光线时必须使用齐次坐标（区分点和方向），否则会发生意外的变换，如做平移变换时会将dir做出错误的变换
	Vec4f orig = Vec4f(r.getOrigin(), 1.0f), dir = Vec4f(r.getDirection(), 0.0f);
	inversedMatrix.Transform(orig);
	inversedMatrix.Transform(dir);
	// 如果要对dir归一化，为了保证距离即dir*t不变，t会相应增大，首先要对tmin，tmax进行同等缩放，在拿到返回的t后还有进行重新缩放
	// 此处由于sphere的intersect算法采用几何法，依赖于空间距离计算，必须保证dir标准化，所以采取了对t缩放的方式
	float tScale = dir.Length();
	dir.Normalize();
	Ray nr = Ray(Vec3f(orig.x(), orig.y(), orig.z()), Vec3f(dir.x(), dir.y(), dir.z()));
	if (object->intersect(nr, h, tScale*tmin, tScale*tmax))
	{
		Vec3f n = h.getNormal();
		//matrix.Transform(n);
		transposedInversedMatrix.Transform(n);
		n.Normalize();
		h.set(h.getT() / tScale, h.getMaterial(), n, r);
		return true;
	}
	return false;
}

bool Transform::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// Transform的intersectShadowRay需要把相关参数委托给object的intersectShadowRay，如果直接委托给intersect，就会无法实现多次碰撞的记录
{ 
	// 变换光线时必须使用齐次坐标（区分点和方向），否则会发生意外的变换，如做平移变换时会将dir做出错误的变换
	Vec4f orig = Vec4f(r.getOrigin(), 1.0f), dir = Vec4f(r.getDirection(), 0.0f);
	inversedMatrix.Transform(orig);
	inversedMatrix.Transform(dir);
	// 如果要对dir归一化，为了保证距离即dir*t不变，t会相应增大，首先要对tmin，tmax进行同等缩放，在拿到返回的t后还有进行重新缩放
	// 此处由于sphere的intersect算法采用几何法，依赖于空间距离计算，必须保证dir标准化，所以采取了对t缩放的方式
	float tScale = dir.Length();
	dir.Normalize();
	Ray nr = Ray(Vec3f(orig.x(), orig.y(), orig.z()), Vec3f(dir.x(), dir.y(), dir.z()));
	if (object->intersectShadowRay(nr, h, tScale*distanceToLight, color))// 这里同样要对distanceToLight进行缩放
	{
		Vec3f n = h.getNormal();
		//matrix.Transform(n);
		transposedInversedMatrix.Transform(n);
		n.Normalize();
		h.set(h.getT() / tScale, h.getMaterial(), n, r);
		//if (h.getMaterial()->getTransparentColor().Length() > EPSILON && r.getDirection().Dot3(h.getNormal()) > 0)color = (1.0f / tScale) * color;
		return true;
	}
	return false;
}

void Transform::paint(void)
{
	glPushMatrix();
	glMultMatrixf(glMatrix);
	object->paint();
	glPopMatrix();
}