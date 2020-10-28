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

	// ����任���boundingbox
	bb = new BoundingBox(*(object->getBoundingBox()));
	Vec3f min = bb->getMin(), max = bb->getMax();
	float nminx = min.x(), nminy = min.y(), nminz = min.z(), nmaxx = max.x(), nmaxy = max.y(), nmaxz = max.z();
	Vec4f vertexes[] = {
		Vec4f(min.x(), min.y(), min.z(), 1.0f),
		Vec4f(min.x(), min.y(), max.z(), 1.0f),
		Vec4f(min.x(), max.y(), min.z(), 1.0f),
		Vec4f(min.x(), max.y(), max.z(), 1.0f),
		Vec4f(max.x(), min.y(), min.z(), 1.0f),
		Vec4f(max.x(), min.y(), max.z(), 1.0f),
		Vec4f(max.x(), max.y(), min.z(), 1.0f),
		Vec4f(max.x(), max.y(), max.z(), 1.0f)
	};
	for (int i = 0; i < 8; i++)
	{
		matrix.Transform(vertexes[i]);
		nminx = nminx > vertexes[i].x() ? vertexes[i].x() : nminx;
		nminy = nminy > vertexes[i].y() ? vertexes[i].y() : nminy;
		nminz = nminz > vertexes[i].z() ? vertexes[i].z() : nminz;
		nmaxx = nmaxx < vertexes[i].x() ? vertexes[i].x() : nmaxx;
		nmaxy = nmaxy < vertexes[i].y() ? vertexes[i].y() : nmaxy;
		nmaxz = nmaxz < vertexes[i].z() ? vertexes[i].z() : nmaxz;
	}
	bb->Set(Vec3f(nminx, nminy, nminz), Vec3f(nmaxx, nmaxy, nmaxz));
}

Transform::~Transform()
{
	delete bb;
	delete object;
	delete[] glMatrix;
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

//bool Transform::intersectShadowRay(const Ray& r, Hit& h, float distanceToLight, Vec3f& color)// Transform��intersectShadowRay��Ҫ����ز���ί�и�object��intersectShadowRay�����ֱ��ί�и�intersect���ͻ��޷�ʵ�ֶ����ײ�ļ�¼
//{ 
//	// �任����ʱ����ʹ��������꣨���ֵ�ͷ��򣩣�����ᷢ������ı任������ƽ�Ʊ任ʱ�Ὣdir��������ı任
//	Vec4f orig = Vec4f(r.getOrigin(), 1.0f), dir = Vec4f(r.getDirection(), 0.0f);
//	inversedMatrix.Transform(orig);
//	inversedMatrix.Transform(dir);
//	// ���Ҫ��dir��һ����Ϊ�˱�֤���뼴dir*t���䣬t����Ӧ��������Ҫ��tmin��tmax����ͬ�����ţ����õ����ص�t���н�����������
//	// �˴�����sphere��intersect�㷨���ü��η��������ڿռ������㣬���뱣֤dir��׼�������Բ�ȡ�˶�t���ŵķ�ʽ
//	float tScale = dir.Length();
//	dir.Normalize();
//	Ray nr = Ray(Vec3f(orig.x(), orig.y(), orig.z()), Vec3f(dir.x(), dir.y(), dir.z()));
//	if (object->intersectShadowRay(nr, h, tScale*distanceToLight, color))// ����ͬ��Ҫ��distanceToLight��������
//	{
//		Vec3f n = h.getNormal();
//		//matrix.Transform(n);
//		transposedInversedMatrix.Transform(n);
//		n.Normalize();
//		h.set(h.getT() / tScale, h.getMaterial(), n, r);
//		//if (h.getMaterial()->getTransparentColor().Length() > EPSILON && r.getDirection().Dot3(h.getNormal()) > 0)color = (1.0f / tScale) * color;
//		return true;
//	}
//	return false;
//}

void Transform::paint(void)
{
	glPushMatrix();
	glMultMatrixf(glMatrix);
	object->paint();
	glPopMatrix();
}

void Transform::insertIntoGrid(Grid* g, Matrix* m)
{
	if (m == nullptr) {
		object->insertIntoGrid(g, &matrix);
	}
	else {
		Matrix new_matrix = (*m) * matrix;
		object->insertIntoGrid(g, &new_matrix);
	}
}