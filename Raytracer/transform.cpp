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
	// �任����ʱ����ʹ��������꣨���ֵ�ͷ��򣩣�����ᷢ������ı任������ƽ�Ʊ任ʱ�Ὣdir��������ı任
	Vec4f orig = Vec4f(r.getOrigin(), 1.0f), dir = Vec4f(r.getDirection(), 0.0f);
	inversedMatrix.Transform(orig);
	inversedMatrix.Transform(dir);
	// ���Ҫ��dir��һ����Ϊ�˱�֤���뼴dir*t���䣬t����Ӧ��������Ҫ��tmin��tmax����ͬ�����ţ����õ����ص�t���н�����������
	// �˴�����sphere��intersect�㷨���ü��η��������ڿռ������㣬���뱣֤dir��׼�������Բ�ȡ�˶�t���ŵķ�ʽ
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

bool Transform::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// Transform��intersectShadowRay��Ҫ����ز���ί�и�object��intersectShadowRay�����ֱ��ί�и�intersect���ͻ��޷�ʵ�ֶ����ײ�ļ�¼
{ 
	// �任����ʱ����ʹ��������꣨���ֵ�ͷ��򣩣�����ᷢ������ı任������ƽ�Ʊ任ʱ�Ὣdir��������ı任
	Vec4f orig = Vec4f(r.getOrigin(), 1.0f), dir = Vec4f(r.getDirection(), 0.0f);
	inversedMatrix.Transform(orig);
	inversedMatrix.Transform(dir);
	// ���Ҫ��dir��һ����Ϊ�˱�֤���뼴dir*t���䣬t����Ӧ��������Ҫ��tmin��tmax����ͬ�����ţ����õ����ص�t���н�����������
	// �˴�����sphere��intersect�㷨���ü��η��������ڿռ������㣬���뱣֤dir��׼�������Բ�ȡ�˶�t���ŵķ�ʽ
	float tScale = dir.Length();
	dir.Normalize();
	Ray nr = Ray(Vec3f(orig.x(), orig.y(), orig.z()), Vec3f(dir.x(), dir.y(), dir.z()));
	if (object->intersectShadowRay(nr, h, tScale*distanceToLight, color))// ����ͬ��Ҫ��distanceToLight��������
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