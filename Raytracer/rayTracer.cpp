#include "rayTracer.h"
#include "rayTree.h"
#include <iomanip>

RayTracer::RayTracer()
{
	width = 200, height = 200;
	scene = nullptr;
	output_mode = false, output_file = nullptr, shade_back = true;
	depth_mode = false, depth_file = nullptr, depth_min = 0, depth_max = FLT_MAX;
	normal_mode = false, normal_file = nullptr;
	image_output = nullptr, image_depth = nullptr, image_normal = nullptr;
	max_bounces = 10, cutoff_weight = 10.0f, shadows = true;
	DEBUG_LOG = false; DEBUG_RAY = false;
}

RayTracer& RayTracer::getInstance()
{
	static RayTracer instance;// �ֲ���̬�����Զ������ͻ��գ����˼·��Ȼ�Ƚ���֣����ǱȽ���Ч
	return instance;
}

//RayTracer::RayTracer(int width, int height, SceneParser* scene)
//{
//	image_output = nullptr;
//	image_depth = nullptr;
//	image_normal = nullptr;
//	initialize(width, height, scene);
//}

void RayTracer::initialize(int width, int height, SceneParser* scene, int max_bounces, float cutoff_weight, bool shadows)
{
	this->width = width;
	this->height = height;
	this->scene = scene;
	this->max_bounces = max_bounces;
	this->cutoff_weight = cutoff_weight;
	this->shadows = shadows;
}

RayTracer::~RayTracer()
{
	if (image_output != nullptr)delete image_output;
	if (image_depth != nullptr)delete image_depth;
	if (image_normal != nullptr)delete image_normal;
}

void RayTracer::setOutput(char *output_file, bool shade_back = false)
{
	this->output_mode = true;
	this->output_file = output_file;
	this->shade_back = shade_back;
	if (image_output != nullptr)delete image_output;
	image_output = new Image(width, height);
}

void RayTracer::setDepth(char *depth_file, float depth_min, float depth_max)
{
	depth_mode = true;
	this->depth_file = depth_file;
	this->depth_min = depth_min;
	this->depth_max = depth_max;
	if (image_depth != nullptr)delete image_depth;
	image_depth = new Image(width, height);
}

void RayTracer::setNormal(char *normal_file)
{
	normal_mode = true;
	this->normal_file = normal_file;
	if (image_normal != nullptr)delete image_normal;
	image_normal = new Image(width, height);
}

void RayTracer::setDebugMode(int mode)
{
	if (mode == 1)
	{
		DEBUG_LOG = true;
	}
	else
	{
		cout << "RAYTRACER::WARNING: Undefined debug mode: " << mode << " for RayTracer" << endl;
	}
}

void RayTracer::renderRayCast(void) 
{
	if (scene == nullptr)
	{
		cout << "RAYTRACER::WARNING: Scene hasn't been specified" << endl;
		return;
	}
	if (output_mode || depth_mode || normal_mode)// ���뱣ָ֤������һ����Ⱦģʽ
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				/*if (i == 266 && j == 130) {
					cout << "pause" << endl;
				}*/
				Vec3f color = scene->getBackgroundColor(), normal = Vec3f();// ���û�����
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(FLT_MAX, nullptr, Vec3f());
				if (DEBUG_LOG)cout << Vec3f(i, j, 0) << ":" << r << " intersect with ";
				if (scene->getGroup()->intersect(r, h, scene->getCamera()->getTMin(), FLT_MAX))
				{
					if (DEBUG_LOG)cout << h.getIntersectionPoint() << " and return " << h << endl;
					assert(h.getMaterial() != nullptr);

					// shade_back
					normal = h.getNormal();
					if (shade_back)
					{
						if (r.getDirection().Dot3(normal) > 0)normal.Negate();
					}
					if (shadows)
					{
						// ������ɫ������Ӱ��
						color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();
						int numLights = scene->getNumLights();
						for (int i = 0; i < numLights; i++)// ����ÿ����Դ�����������
						{
							Light* light = scene->getLight(i);
							Vec3f p = h.getIntersectionPoint(), dir, col;
							float dis = 0.0f;
							light->getIllumination(p, dir, col, dis);
							// �����ڵ�
							Ray rayShadow(p, dir);
							Hit hitShadow(dis, nullptr, Vec3f());
							if (!scene->getGroup()->intersectShadowRay(rayShadow, hitShadow, dis, col))
								color += h.getMaterial()->Shade(r, h, dir, col, dis);
							if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "RayTracer::WARNING: color at pixel(" << i << "," << j << ") is out of range" << endl;
							if (DEBUG_LOG)cout << "Color:" << color << endl;
						}
					}
					else
					{
						// ������ɫ������Ӱ��
						color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();// ���㻷����
						int numLights = scene->getNumLights();
						for (int i = 0; i < numLights; i++)// ����ÿ����Դ�����������
						{
							Light* light = scene->getLight(i);
							Vec3f p = h.getIntersectionPoint(), dir, col;
							float dis = 0.0f;
							light->getIllumination(p, dir, col, dis);
							color += h.getMaterial()->Shade(r, h, dir, col, dis);
							if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "RayTracer::WARNING: color at pixel("<<i<<","<<j<<") is out of range" << endl;
							if (DEBUG_LOG)cout << "Color:" << color << endl;
						}
					}
				}
				else
				{
					if (DEBUG_LOG)cout << "nothing" << endl;
				}
				if (output_mode)
				{
					image_output->SetPixel(i, j, color);
				}
				if (depth_mode)
				{
					float depth = 1.0f - (h.getT() - depth_min) / (depth_max - depth_min);
					image_depth->SetPixel(i, j, Vec3f(depth, depth, depth));
				}
				if (normal_mode)
				{
					Vec3f n = normal;
					n.Set(fabs(n.r()), fabs(n.g()), fabs(n.b()));
					image_normal->SetPixel(i, j, n);
				}
			}
		}
		if (output_mode)image_output->SaveTGA(output_file);
		if (depth_mode)image_depth->SaveTGA(depth_file);
		if (normal_mode)image_normal->SaveTGA(normal_file);
	} else cout << "RayTracer::WARNING: No render mode is specified(output,depth,normal)" << endl;
}

Vec3f inline mirrorDirection(const Vec3f& normal, const Vec3f& incoming)
{
	return incoming - 2 * incoming.Dot3(normal)*normal;
}

bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming,
	float index_i, float index_t, Vec3f& transmitted)
{
	const float eta = index_i / index_t, normalDotIncoming = normal.Dot3(incoming), 
		judgement = (1 - eta * eta * (1 - normalDotIncoming * normalDotIncoming));
	if (judgement < 0)return false;// ����ȫ����
	transmitted = (eta * normalDotIncoming - sqrt(judgement)) * normal - eta * incoming;
	transmitted.Normalize();
	return true;
}

Vec3f RayTracer::traceRay(Ray& r, float tmin, int bounces, float weight, float indexOfRefraction, Hit& h) const
{
	if (bounces > max_bounces || weight < cutoff_weight)return Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f color = scene->getBackgroundColor();
	if (scene->getGroup()->intersect(r, h, tmin, FLT_MAX))
	{
		bool inside = false;// ����ж��Ǵ������ڲ�������ߣ�����insideΪtrue
		if (shade_back && r.getDirection().Dot3(h.getNormal()) > 0)
		{
			Vec3f normal = h.getNormal();
			normal.Negate();
			h.set(h.getT(), h.getMaterial(), normal, r);
			inside = true;
		}
		color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();
		int numLights = scene->getNumLights();
		for (int i = 0; i < numLights; i++)// ����ÿ����Դ��Ӧ����Ӱ
		{
			Light* light = scene->getLight(i);
			Vec3f intersectedPoint = h.getIntersectionPoint(), directionToLight, lightColor; float distanceToLight;
			light->getIllumination(intersectedPoint, directionToLight, lightColor, distanceToLight);
			Ray rayShadow(intersectedPoint, directionToLight);
			Hit hitShadow(distanceToLight, nullptr, Vec3f());
			if (shadows)// �ȼ���Ƿ���Ҫ��Ⱦ��Ӱ
			{
				scene->getGroup()->intersectShadowRay(rayShadow, hitShadow, distanceToLight, lightColor);// ��ȡ�Ƿ��ڵ�
				if(!isfinite(hitShadow.getT()) || fabs(hitShadow.getT() - distanceToLight) < EPSILON)
					color += h.getMaterial()->Shade(r, h, directionToLight, lightColor, distanceToLight);// ���û�б���ȫ�ڵ�(δ���ڵ���͸�������ڵ�������Phong���ʽ�����ɫ
			}

			if (DEBUG_RAY) RayTree::AddShadowSegment(rayShadow, EPSILON, hitShadow.getT());
		}
		if (h.getMaterial()->getReflectColor().Length() > EPSILON)// ������з������
		{
			Ray rayReflection(h.getIntersectionPoint(), mirrorDirection(h.getNormal(), r.getDirection()));//����ײ�㷢���µ�����
			Hit hitReflection(FLT_MAX, nullptr, Vec3f());
			color += h.getMaterial()->getReflectColor() * traceRay( rayReflection, EPSILON, bounces + 1, // ���µ����߽��е�������
				weight * h.getMaterial()->getReflectColor().Length(), h.getMaterial()->getIndexOfRefraction(), hitReflection);

			if (DEBUG_RAY) RayTree::AddReflectedSegment(rayReflection, EPSILON, hitReflection.getT());
		}
		if (h.getMaterial()->getTransparentrColor().Length() > EPSILON)// �����������/͸������
		{
			Vec3f transmittedDir;
			float indexT = inside ? 1.0f : h.getMaterial()->getIndexOfRefraction();// ������ڲ����򽫳��������������Ϊ1.0���ݲ����ǲ�ͬ����֮������䣬ֻ���ǽ��ʺ���գ�
			if (transmittedDirection(h.getNormal(), -1.0f * r.getDirection(),
				indexOfRefraction, indexT, transmittedDir))// ����Ƿ��ܹ�������������,������ȫ���䣬���䲿����Ч
			{
				Ray rayTransparent(h.getIntersectionPoint(), transmittedDir);
				Hit hitTransparent(FLT_MAX, nullptr, Vec3f());// ����ײ�㷢��������ߵ�������
				color += h.getMaterial()->getTransparentrColor() * traceRay(rayTransparent, EPSILON, bounces + 1,
					weight * h.getMaterial()->getTransparentrColor().Length(), indexT, hitTransparent);

				if (DEBUG_RAY) RayTree::AddTransmittedSegment(rayTransparent, EPSILON, hitTransparent.getT());
			}

		}
	}
	return color;
}

void RayTracer::renderRayTracing(void)
{
	if (scene == nullptr)
	{
		cout << "RAYTRACER::WARNING: Scene hasn't been specified" << endl;
		return;
	}
	if (output_mode || depth_mode || normal_mode)// ���뱣ָ֤������һ����Ⱦģʽ
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{

				if (i == 100 && j == 40) {
					cout << "pause" << endl;
				}
				cout << "\r" << fixed << setprecision(2) << 100.0f * (i * width + j + 1) / (width * height) << "%";
				Vec3f color(0.0f, 0.0f, 0.0f);// ���û�����
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(FLT_MAX, nullptr, Vec3f());
				if (DEBUG_LOG)cout << Vec3f(i, j, 0) << ":" << r << " return ";
				color += traceRay(r, scene->getCamera()->getTMin(), 0, 1.0f, 1.0f, h);// ���й��߸���
				//if (i == 100 && j == 40) color = Vec3f(1.0f, 1.0f, 1.0f);
				//if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "RayTracer::WARNING: color at pixel(" << i << "," << j << ") is out of range" << endl;
				if (output_mode)
				{
					image_output->SetPixel(i, j, color);
				}
				if (depth_mode)
				{
					float depth = 1.0f - (h.getT() - depth_min) / (depth_max - depth_min);
					image_depth->SetPixel(i, j, Vec3f(depth, depth, depth));
				}
				if (normal_mode)
				{
					Vec3f n = h.getNormal();
					//if (shade_back && r.getDirection().Dot3(n) < 0)n.Negate();
					n.Set(fabs(n.r()), fabs(n.g()), fabs(n.b()));
					image_normal->SetPixel(i, j, n);
				}
				if (DEBUG_LOG)cout << "Color:" << color << endl;
			}
		}
		if (output_mode)image_output->SaveTGA(output_file);
		if (depth_mode)image_depth->SaveTGA(depth_file);
		if (normal_mode)image_normal->SaveTGA(normal_file);
	}
	else cout << "RayTracer::WARNING: No render mode is specified(output,depth,normal)" << endl;
}

void RayTracer::renderRayDebug(float x, float y)
{
	Ray r = scene->getCamera()->generateRay(Vec2f(x, y));
	Hit h(FLT_MAX, nullptr, Vec3f());
	if (DEBUG_LOG)cout << Vec3f(x, y, 0) << ":" << r << " ray debugging";
	DEBUG_RAY = true; 
	traceRay(r, scene->getCamera()->getTMin(), 0, 1.0f, 1.0f, h);
	RayTree::SetMainSegment(r, 0, h.getT());
	DEBUG_RAY = false;
}