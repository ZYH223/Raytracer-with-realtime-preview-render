#define _USE_MATH_DEFINES
#include <math.h>
#define deg2rad(x) ((M_PI * x) / 180.0f)
#include "sphere.h"
float Sphere::theta, Sphere::phi;// data for tessellation
bool Sphere::gouraud;

Sphere::Sphere(Vec3f c, float r, Material* m)
{
	radius = r;
	center = c;
	//material =  new Material(*m);
	material = m;
	const Vec3f temp(radius, radius, radius);
	bb = new BoundingBox(center - temp, center + temp);
	// default initiallize the parameters for tessellation
	/*theta = deg2rad(36.0f);
	phi = deg2rad(18.0f);*/
}

Sphere::~Sphere()
{
	//释放material的工作交给scene_parser来做
	delete bb;
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

void Sphere::insertIntoGrid(Grid* g, Matrix* m)
{
	Vec3f length = g->GetLength();
	float lenx = length.x(), leny = length.y(), lenz = length.z();
	// 怎样高效地指出占据的网格？（遍历代价有点高）
	float mx = g->CellNumX(), my = g->CellNumY(), mz = g->CellNumZ();
	//float mx = (floor(radius / lenx) + 0.5f) * lenx, my = (floor(radius / leny) + 0.5f) * leny, mz = (floor(radius / lenz) + 0.5f) * lenz;
	/*for (float x = mx < g->getBoundingBox(); i < )*/
	for (int i = 0; i < mx; i++)
	{
		for (int j = 0; j < my; j++)
		{
			for (int k = 0; k < mz; k++)
			{
				Vec3f cellCenter((i - mx/2.0f + 0.5f) * lenx, (j - my/2.0f + 0.5f) * leny, (k - mz/2.0f + 0.5f) * lenz);// 计算当前单元格与球心的距离
				if ((cellCenter - center).Length() - radius < EPSILON)g->SetCell(i, j, k, true);
			}
		}
	}
}

void Sphere::SetTessellationParameters(int theta_step, int phi_step, bool isgouraud = false)
{
	Sphere::theta = deg2rad(360.0f/ theta_step);
	Sphere::phi = deg2rad(180.0f/phi_step);
	Sphere::gouraud = isgouraud;
}

void Sphere::paint(void)
{
	// shading
	material->glSetMaterial();
	// geometry
	glBegin(GL_QUADS);
	for (float iPhi = deg2rad (-90.0f); iPhi <= deg2rad(90.0f); iPhi += phi)
	{// defined as pitch in [-90,90](from the positive part of y axis to the negative one)
		for (float iTheta = deg2rad(0.0f); iTheta < deg2rad(360.0f); iTheta += theta)
		{// defined as yaw in [0,360)(from the positive part of x axis and rotate counter-clockwisely)
			//// compute appropriate coordinates & normals
			//float nx = cos(iTheta)*cos(iPhi), ny = sin(iTheta)*cos(iPhi), nz = sin(iPhi);
			//Vec3f normal = Vec3f(nx, ny, nz), b1, b2;
			//if (normal == Vec3f(1.0f, 0.0f, 0.0f)) Vec3f::Cross3(b1, normal, Vec3f(0.0f, 1.0f, 0.0f));
			//else Vec3f::Cross3(b1, normal, Vec3f(1.0f, 0.0f, 0.0f));
			//Vec3f::Cross3(b2, normal, b1);
			//b1.Normalize(); b2.Normalize();
			////float height = 0.1*radius * sin(phi / 2), width = 0.1*radius * sin(theta / 2);
			//float k = sqrt(sin(iPhi + phi / 2)*sin(iPhi + phi / 2)*tan(theta / 2)*tan(theta / 2) + tan(phi / 2)*tan(phi / 2));
			//float D = (sqrt(k*k + 4 * radius*radius) - k) / 2;
			//float width = sin(iPhi + phi / 2)*tan(theta / 2)*D, height = tan(phi / 2) * D;
			//Vec3f point[4] = { center + (normal * center.Length() - width * b1 - height * b2),
			//	center + (normal * D + width * b1 - height * b2),
			//	center + (normal * D + width * b1 + height * b2),
			//	center + (normal * D - width * b1 + height * b2)
			//};
			float nx = 0.0f, ny = 0.0f, nz = 0.0f;
			GLfloat point[4][3] = 
			{ 
				{ sin(iTheta)*cos(iPhi), sin(iPhi), cos(iTheta)*cos(iPhi) },
				{ sin(iTheta)*cos(iPhi + phi), sin(iPhi + phi), cos(iTheta)*cos(iPhi + phi) },
				{ sin(iTheta + theta)*cos(iPhi + phi), sin(iPhi + phi), cos(iTheta + theta)*cos(iPhi + phi) },
				{ sin(iTheta + theta)*cos(iPhi), sin(iPhi), cos(iTheta + theta)*cos(iPhi) }
			};
			// 直接用坐标计算的法线需要经过翻转才能正常显示
			GLfloat normal[4][3];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					normal[i][j] = -point[i][j];
				}
			}
			for (int i = 0; i < 4; i++)
			{
				nx += point[i][0] / 4;
				ny += point[i][1] / 4;
				nz += point[i][2] / 4;
				point[i][0] = radius * point[i][0] + center.x();
				point[i][1] = radius * point[i][1] + center.y();
				point[i][2] = radius * point[i][2] + center.z();
			}
			float l = sqrt(nx * nx + ny * ny + nz * nz);
			nx = -nx / l, ny = -ny / l, nz = -nz / l;

			// send gl vertex commands
			if (gouraud) 
			{
				for (int i = 0; i < 4; i++) 
				{
					glNormal3fv(normal[i]);
					glVertex3fv(point[i]);
					/*glNormal3f((point[i].x() - center.x()) / radius, (point[i].y() - center.y()) / radius, (point[i].z() - center.z()) / radius);
					glVertex3f(point[i].x(), point[i].y(), point[i].z());*/
				}
			}
			else 
			{
				glNormal3f(nx, ny, nz);
				for (int i = 0; i < 4; i++)
				{
					glVertex3fv(point[i]);
				}
				/*glNormal3f(nx, ny, nz);
				for (int i = 0; i < 4; i++)
					glVertex3f(point[i].x(), point[i].y(), point[i].z());*/
			}
		}
	}
	glEnd();
}