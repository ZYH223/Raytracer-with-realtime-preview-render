#include "transform.h"

Transform::Transform(Matrix& m, Object3D* o)
{
	matrix = m;
	object = o; 
	inversedMatrix = matrix;
	inversedMatrix.Inverse(DIFF);
	transposedInversedMatrix = inversedMatrix;
	transposedInversedMatrix.Transpose();
}

Transform::~Transform()
{
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