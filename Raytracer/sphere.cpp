#include "sphere.h"

Sphere::Sphere(Vec3f c, float r, Material* m)
{
	radius = r;
	center = c;
	//material =  new Material(*m);
	material = m;

	// default initiallize the parameters for tessellation
	theta = 36.0f;
	phi = 18.0f;
}

Sphere::~Sphere()
{
	//释放内存的工作交给scene_parser来做
	//delete material;
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

void Sphere::SetTessellationParameters(float theta, float phi, bool gouraud = false)
{
	this->theta = theta;
	this->phi = phi;
	this->gouraud = gouraud;
}

void Sphere::paint(void)
{
	// shading
	material->glSetMaterial();
	// geometry
	glBegin(GL_QUADS);
	for (float iPhi = -90.0f; iPhi <= 90.0f; iPhi += phi)
	{// defined as pitch in [-90,90](from the positive part of y axis to the negative one)
		for (float iTheta = 0.0f; iTheta < 360.0f; iTheta += theta)
		{// defined as yaw in [0,360)(from the positive part of x axis and rotate counter-clockwisely)
			// compute appropriate coordinates & normals
			float nx = cos(iTheta)*cos(iPhi), ny = sin(iTheta)*cos(iPhi), nz = sin(iPhi);
			Vec3f normal = Vec3f(nx, ny, nz), b1, b2;
			if (normal == Vec3f(1.0f, 0.0f, 0.0f)) Vec3f::Cross3(b1, normal, Vec3f(0.0f, 1.0f, 0.0f));
			else Vec3f::Cross3(b1, normal, Vec3f(1.0f, 0.0f, 0.0f));
			Vec3f::Cross3(b2, normal, b1);
			b1.Normalize(); b2.Normalize();
			float height = radius * sin(phi / 2), width = radius * sin(theta / 2);
			Vec3f point[4] = { (normal * center.Length() - width * b1 - height * b2),
				(normal * center.Length() - width * b1 + height * b2),
				(normal * center.Length() + width * b1 - height * b2),
				(normal * center.Length() + width * b1 + height * b2)
			};

			// send gl vertex commands
			if (gouraud) 
			{
				for (int i = 0; i < 4; i++) 
				{
					glNormal3f((point[i].x() - center.x()) / radius, (point[i].y() - center.y()) / radius, (point[i].z() - center.z()) / radius);
					glVertex3f(point[i].x(), point[i].y(), point[i].z());
				}
			}
			else 
			{
				glNormal3f(nx, ny, nz);
				for (int i = 0; i < 4; i++)
					glVertex3f(point[i].x(), point[i].y(), point[i].z());
			}
		}
	}
	glEnd();
}