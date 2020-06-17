#include <iostream>
#include "scene_parser.h"
#include "image.h"
#include "light.h"
#include "ray.h"
#include "hit.h"
#include "group.h"
#include "camera.h"
#include "matrix.h"
using namespace std;
const bool DEBUG = false;
const bool DEBUG_LOG = false;

int main(int argc, char *argv[]) {
	char *input_file = nullptr;
	int width = 100;
	int height = 100;
	char *output_file = nullptr;
	float depth_min = -FLT_MAX;
	float depth_max = FLT_MAX;
	char *depth_file = nullptr;
	char *normal_file = nullptr;
	bool diffuse_mode = false, depth_mode = false, normal_mode = false, shade_back = false;

	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			width = atoi(argv[i]);
			i++; assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
			diffuse_mode = true;
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
			depth_mode = true;
		}
		else if (!strcmp(argv[i], "-normals")) {
			i++; assert(i < argc);
			normal_file = argv[i];
			normal_mode = true;
		}
		else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = true;
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	// 以下用于调试程序时模拟输入参数
	if (DEBUG) 
	{
		const int filename_buffer_length = 64;
		input_file = new char[filename_buffer_length];
		strcpy_s(input_file, filename_buffer_length, "Input/scene2_05_inside_sphere.txt");
		width = 200, height = 200;
		if (DEBUG_LOG) 
		{
			width = 32, height = 32;
		}
		diffuse_mode = true, depth_mode = false, normal_mode = false, shade_back = true;
		if (diffuse_mode) 
		{
			output_file = new char[filename_buffer_length];
			strcpy_s(output_file, filename_buffer_length, "Output/output2_05.tga");
		}
		if (depth_mode) 
		{
			depth_min = 9;
			depth_max = 11;
			depth_file = new char[filename_buffer_length];
			strcpy_s(depth_file, filename_buffer_length, "Output/depth2_05.tga");
		}
		if (normal_mode) 
		{
			normal_file = new char[filename_buffer_length];
			strcpy_s(normal_file, filename_buffer_length, "Output/normals2_05.tga");
		}
	}

	SceneParser scene(input_file);
	Image image(width, height);
	Image image_depth(width, height);
	Image image_normal(width, height);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			/*if (i == 31 && j == 0) {
				cout << "start" << endl;
			}*/
			Vec3f color = scene.getBackgroundColor(), normal = Vec3f();
			Ray r = scene.getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
			Hit h(depth_max, nullptr, Vec3f());
			if (DEBUG_LOG)cout << Vec3f(i, j, 0) << ":" << r << " intersect with ";
			if (scene.getGroup()->intersect(r, h, scene.getCamera()->getTMin(), FLT_MAX))
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
				color = scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();// 计算环境光
				int numLights = scene.getNumLights();
				for (int i = 0; i < numLights; i++)// 计算每个光源的漫反射分量
				{
					Light* light = scene.getLight(i);
					Vec3f p = h.getIntersectionPoint(), dir, col;
					light->getIllumination(p, dir, col);
					float d = dir.Dot3(normal);
					if (d > 0)// d<=0时没有漫反射分量
					{
						color += h.getMaterial()->getDiffuseColor() * d * col;
					}
				}
			}
			else
			{
				if (DEBUG_LOG)cout << "nothing" << endl;
			}
			if (diffuse_mode) 
			{
				image.SetPixel(i, j, color);
			}
			if (depth_mode)
			{
				float depth = 1.0f - (h.getT() - depth_min) / (depth_max - depth_min);
				image_depth.SetPixel(i, j, Vec3f(depth, depth, depth));
			}
			if (normal_mode)
			{
				Vec3f n = normal;
				n.Set(fabs(n.r()), fabs(n.g()), fabs(n.b()));
				image_normal.SetPixel(i, j, n);
			}
		}
	}
	if (diffuse_mode)image.SaveTGA(output_file);
	if (depth_mode)image_depth.SaveTGA(depth_file);
	if (normal_mode)image_normal.SaveTGA(normal_file);

	if (DEBUG)
	{
		delete[] input_file;
		delete[] output_file;
		delete[] depth_file;
		system("pause");
	}
	return 0;
}