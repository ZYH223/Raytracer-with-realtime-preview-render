#include "rayTracer.h"
#include "rayTree.h"
//#include "matrix.h"
#include <iomanip>

RayTracer::RayTracer()
{
	width = 200, height = 200;
	scene = nullptr;
	grid = nullptr;
	output_mode = false, output_file = nullptr, shade_back = true;
	depth_mode = false, depth_file = nullptr, depth_min = 0, depth_max = FLT_MAX;
	normal_mode = false, normal_file = nullptr;
	image_output = nullptr, image_depth = nullptr, image_normal = nullptr;
	max_bounces = 10, cutoff_weight = 10.0f, shadows = true;
	grid_visualize = false;
	DEBUG_LOG = false; DEBUG_RAY = false;
}

RayTracer& RayTracer::getInstance()
{
	static RayTracer instance;// 局部静态变量自动创建和回收，用于实现单例，设计思路虽然比较奇怪，但是简单有效
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
	if (grid != nullptr)delete grid;
}

void RayTracer::setGrid(int nx, int ny, int nz, bool grid_visualize)
{
	if (grid == nullptr) {
		grid = new Grid(this->scene->getGroup()->getBoundingBox(), nx, ny, nz);
	}
	else {
		std::cout << "[WARNING]RayTracer::setGrid Redundant grid setup" << std::endl;
	}

	// for debugging and print log
	if (grid_visualize && scene != nullptr) {
		this->grid_visualize = grid_visualize;
		/*float data[16] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};*/
		//Matrix* matrix = new Matrix(data);
		scene->getGroup()->insertIntoGrid(grid, nullptr);// The second param is not avaliable now
		//delete matrix;
		//grid->print();
	}
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
		cout << "[WARNING](RayTracer::setDebugMode) Undefined debug mode: " << mode << " for RayTracer" << endl;
	}
}

Grid* RayTracer::getGrid()
{
	return grid;
}

void  RayTracer::render()
{
	if (grid_visualize) {// 网格渲染模式
		renderRayGrid();
	}
	else {
		renderRayTracing();
	}
}

void RayTracer::renderRayGrid(void)
{
	if (scene == nullptr)
	{
		cout << "[ERROR](RayTracer::renderRayCast) Scene hasn't been specified" << endl;
		return;
	}
	if (output_mode || depth_mode || normal_mode)// 必须保证指定至少一种渲染模式
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				Vec3f color = scene->getBackgroundColor(), normal = Vec3f();// 设置环境光
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(FLT_MAX, nullptr, Vec3f());
				if (DEBUG_LOG)cout << "[DEBUG]RayTracer::renderRayCast " << Vec3f((float)i, (float)j, 0) << ":" << r << " intersect with ";
				if (grid->intersect(r, h, scene->getCamera()->getTMin(), FLT_MAX))
				{
					if (!DEBUG_LOG)cout << "\r" << fixed << setprecision(2) << 100.0f * (i * width + j + 1) / (width * height) << "%";
					if (DEBUG_LOG)cout << h.getIntersectionPoint() << " and return " << h << endl;
					assert(h.getMaterial() != nullptr);

					// shade_back
					normal = h.getNormal();
					if (shade_back)
					{
						if (r.getDirection().Dot3(normal) > 0)normal.Negate();
					}
					// 计算颜色（无阴影）
					color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();// 计算环境光
					int numLights = scene->getNumLights();
					for (int i = 0; i < numLights; i++)// 计算每个光源的漫反射分量
					{
						Light* light = scene->getLight(i);
						Vec3f p = h.getIntersectionPoint(), dir, col;
						float dis = 0.0f;
						light->getIllumination(p, dir, col, dis);
						color += h.getMaterial()->Shade(r, h, dir, col, dis);
						if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "[WARNING](RayTracer::renderRayCast) color at pixel(" << i << "," << j << ") is out of range" << endl;
						if (DEBUG_LOG)cout << "Color:" << color << endl;
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
	}
	else cout << "[WARNING](RayTracer::renderRayCast) No render mode is specified(output,depth,normal)" << endl;
}

// this function was replaced by renderRayTracing
void RayTracer::renderRayCast(void) 
{
	if (scene == nullptr)
	{
		cout << "[ERROR](RayTracer::renderRayCast) Scene hasn't been specified" << endl;
		return;
	}
	if (output_mode || depth_mode || normal_mode)// 必须保证指定至少一种渲染模式
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				/*if (i == 266 && j == 130) {
					cout << "pause" << endl;
				}*/
				Vec3f color = scene->getBackgroundColor(), normal = Vec3f();// 设置环境光
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(FLT_MAX, nullptr, Vec3f());
				if (DEBUG_LOG)cout << "[DEBUG]RayTracer::renderRayCast " << Vec3f((float)i, (float)j, 0) << ":" << r << " intersect with ";
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
						// 计算颜色（有阴影）
						color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();
						int numLights = scene->getNumLights();
						for (int i = 0; i < numLights; i++)// 计算每个光源的漫反射分量
						{
							Light* light = scene->getLight(i);
							Vec3f p = h.getIntersectionPoint(), dir, col;
							float dis = 0.0f;
							light->getIllumination(p, dir, col, dis);
							// 计算遮挡
							Ray rayShadow(p, dir);
							Hit hitShadow(dis, nullptr, Vec3f());
							if (!scene->getGroup()->intersectShadowRay(rayShadow, hitShadow, dis, col))
								color += h.getMaterial()->Shade(r, h, dir, col, dis);
							if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "[WARNING](RayTracer::renderRayCast) color at pixel(" << i << "," << j << ") is out of range" << endl;
							if (DEBUG_LOG)cout << "Color:" << color << endl;
						}
					}
					else
					{
						// 计算颜色（无阴影）
						color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();// 计算环境光
						int numLights = scene->getNumLights();
						for (int i = 0; i < numLights; i++)// 计算每个光源的漫反射分量
						{
							Light* light = scene->getLight(i);
							Vec3f p = h.getIntersectionPoint(), dir, col;
							float dis = 0.0f;
							light->getIllumination(p, dir, col, dis);
							color += h.getMaterial()->Shade(r, h, dir, col, dis);
							if (color.r() > 1.0f || color.g() > 1.0f || color.b() > 1.0f)cout << "[WARNING](RayTracer::renderRayCast) color at pixel("<<i<<","<<j<<") is out of range" << endl;
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
	} else cout << "[WARNING](RayTracer::renderRayCast) No render mode is specified(output,depth,normal)" << endl;
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
	if (judgement < 0)return false;// 发生全反射
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
		bool inside = false;// 如果判断是从物体内部计算光线，则设inside为true
		if (shade_back && r.getDirection().Dot3(h.getNormal()) > 0)
		{
			Vec3f normal = h.getNormal();
			normal.Negate();
			h.set(h.getT(), h.getMaterial(), normal, r);
			inside = true;
		}
		color = scene->getAmbientLight() * h.getMaterial()->getDiffuseColor();
		int numLights = scene->getNumLights();
		for (int i = 0; i < numLights; i++)// 计算每个光源对应的阴影
		{
			Light* light = scene->getLight(i);
			Vec3f intersectedPoint = h.getIntersectionPoint(), directionToLight, lightColor; float distanceToLight;
			light->getIllumination(intersectedPoint, directionToLight, lightColor, distanceToLight);
			Ray rayShadow(intersectedPoint, directionToLight);
			Hit hitShadow(distanceToLight, nullptr, Vec3f());
			if (shadows)// 先检查是否需要渲染阴影
			{
				bool intersected = scene->getGroup()->intersectShadowRay(rayShadow, hitShadow, distanceToLight, lightColor);// 获取是否命中
				if(!intersected || hitShadow.getMaterial()->getTransparentColor().Length() > EPSILON/*isinf(hitShadow.getT()) || fabs(hitShadow.getT() - distanceToLight) < EPSILON*/)// 如果未命中或者未遮挡，则用Phong材质进行着色（实际上只要t = distanceToLight也就说有光线能够到达光源即可）
					color += h.getMaterial()->Shade(r, h, directionToLight, lightColor, distanceToLight);// 如果没有被完全遮挡(未被遮挡或被透明材质遮挡），
			}

			if (DEBUG_RAY) RayTree::AddShadowSegment(rayShadow, EPSILON, hitShadow.getT());
		}
		if (h.getMaterial()->getReflectColor().Length() > EPSILON)// 如果具有反射材质
		{
			Ray rayReflection(h.getIntersectionPoint(), mirrorDirection(h.getNormal(), r.getDirection()));//从碰撞点发出新的射线
			Hit hitReflection(FLT_MAX, nullptr, Vec3f());
			Vec3f reflect = h.getMaterial()->getReflectColor();
			Vec3f shilick = reflect + (Vec3f(1.0f, 1.0f, 1.0f) - reflect) * pow(1 - rayReflection.getDirection().Dot3(h.getNormal()), 1);
			//cout << shilick << "-" << reflect << endl;
			color += shilick * traceRay( rayReflection, EPSILON, bounces + 1, // 用新的射线进行迭代跟踪
				weight * shilick.Length(), h.getMaterial()->getIndexOfRefraction(), hitReflection);

			if (DEBUG_RAY) RayTree::AddReflectedSegment(rayReflection, EPSILON, hitReflection.getT());
		}
		if (h.getMaterial()->getTransparentColor().Length() > EPSILON)// 如果具有折射/透明材质
		{
			Vec3f transmittedDir;
			float indexT = inside ? 1.0f : h.getMaterial()->getIndexOfRefraction();// 如果是内部，则将出射介质折射率设为1.0（暂不考虑不同介质之间的折射，只考虑介质和真空）
			if (transmittedDirection(h.getNormal(), -1.0f * r.getDirection(),
				indexOfRefraction, indexT, transmittedDir))// 检查是否能够正常发生折射,否则发生全反射，折射部分无效
			{
				Ray rayTransparent(h.getIntersectionPoint(), transmittedDir);
				Hit hitTransparent(FLT_MAX, nullptr, Vec3f());// 从碰撞点发起折射光线迭代跟踪
				color += h.getMaterial()->getTransparentColor() * traceRay(rayTransparent, EPSILON, bounces + 1,
					weight * h.getMaterial()->getTransparentColor().Length(), indexT, hitTransparent);

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
		cout << "[WARNING](RayTracer::renderRayTracing) Scene hasn't been specified" << endl;
		return;
	}
	if (output_mode || depth_mode || normal_mode)// 必须保证指定至少一种渲染模式
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{

				/*if (i == 0 && j == 15) {
					cout << "pause" << endl;
				}*/
				if (!DEBUG_LOG)cout << "\r" << fixed << setprecision(2) << 100.0f * (i * width + j + 1) / (width * height) << "%";
				Vec3f color(0.0f, 0.0f, 0.0f);
				Ray r = scene->getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
				Hit h(FLT_MAX, nullptr, Vec3f());
				if (DEBUG_LOG)cout << Vec3f((float)i, (float)j, 0) << ":" << r << " return ";
				color += traceRay(r, scene->getCamera()->getTMin(), 0, 1.0f, 1.0f, h);// 进行光线跟踪
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
	else cout << "[WARNING](RayTracer::renderRayTracing) No render mode is specified(output,depth,normal)" << endl;
}

void RayTracer::renderDebug(float x, float y)
{
	Ray r = scene->getCamera()->generateRay(Vec2f(x, y));
	Hit h(FLT_MAX, nullptr, Vec3f());
	if (DEBUG_LOG)cout << "[DEBUG]RayTracer::renderRayDebug " << Vec3f(x, y, 0) << ":" << r << " ray debugging";
	DEBUG_RAY = true;
	if (grid_visualize) {
		grid->intersect(r, h, scene->getCamera()->getTMin(), FLT_MAX);
		RayTree::SetMainSegment(r, 0, FLT_MAX);
	}
	else {
		traceRay(r, scene->getCamera()->getTMin(), 0, 1.0f, 1.0f, h);
		RayTree::SetMainSegment(r, 0, h.getT());
	}
	DEBUG_RAY = false;
}