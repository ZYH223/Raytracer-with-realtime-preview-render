#include "rayTracer.h"

RayTracer::RayTracer()
{
	image_output = nullptr;
	image_depth = nullptr;
	image_normal = nullptr;
}

RayTracer& RayTracer::getInstance()
{
	static RayTracer instance;// 局部静态变量自动创建和回收，设计思路虽然比较奇怪，但是比较有效
	return instance;
}

//RayTracer::RayTracer(int width, int height, SceneParser* scene)
//{
//	image_output = nullptr;
//	image_depth = nullptr;
//	image_normal = nullptr;
//	initialize(width, height, scene);
//}

void RayTracer::initialize(int width, int height, SceneParser* scene)
{
	this->width = width;
	this->height = height;
	this->scene = scene;
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
	this->depth_file = depth_file;
	this->depth_min = depth_min;
	this->depth_max = depth_max;
	if (image_depth != nullptr)delete image_depth;
	image_depth = new Image(width, height);
}

void RayTracer::setNormal(char *normal_file)
{
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
		cout << "WARNING: Undefined debug mode for RayTracer" << endl;
	}
}

void RayTracer::render(void) 
{
	if (output_mode || depth_mode || normal_mode)
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				/*if (i == 266 && j == 130) {
					cout << "pause" << endl;
				}*/
				Vec3f color = scene->getBackgroundColor(), normal = Vec3f();
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(depth_max, nullptr, Vec3f());
				if (DEBUG_LOG)cout << Vec3f(i, j, 0) << ":" << r << " intersect with ";
				if (scene->getGroup()->intersect(r, h, scene->getCamera()->getTMin(), FLT_MAX))
				{
					if (DEBUG_LOG)cout << h.getIntersectionPoint() << " and return " << h << endl;
					assert(h.getMaterial() != nullptr);

					// shade_back
					normal = h.getNormal();
					if (shade_back)
					{
						if (r.getDirection().Dot3(normal) > 0)
						{
							normal.Negate();
						}
					}

					// 计算颜色
					color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();// 计算环境光
					int numLights = scene->getNumLights();
					for (int i = 0; i < numLights; i++)// 计算每个光源的漫反射分量
					{
						Light* light = scene->getLight(i);
						Vec3f p = h.getIntersectionPoint(), dir, col;
						float dis = 0.0f;
						light->getIllumination(p, dir, col, dis);
						color += h.getMaterial()->Shade(r, h, dir, col);
						//if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "WARNING: color at pixel("<<i<<","<<j<<") is out of range" << endl;
						if (DEBUG_LOG)cout << "Color:" << color << endl;
						//float d = dir.Dot3(normal);
						//if (d > 0)// d<=0时没有漫反射分量
						//{
						//	color += h.getMaterial()->getDiffuseColor() * d * col;
						//}
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