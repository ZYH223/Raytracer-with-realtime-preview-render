#include <iostream>
#include "glCanvas.h"
#include "rayTracer.h"
#include "Sphere.h"
using namespace std;
const bool DEBUG = true;
const bool DEBUG_LOG = false;

void handleParameter(
	int &argc, char *argv[],
	char *&input_file,
	int &width, int &height,
	char *&output_file,
	char *&depth_file, float &depth_min, float &depth_max,
	char *&normal_file,
	bool &diffuse_mode, bool &depth_mode, bool &normal_mode, bool &shade_back,
	int &theta, int &phi,
	bool &gui, bool &gouraud,
	int &max_bounces, float &cutoff_weight, bool &shadows)
{
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
			depth_min = (float)atof(argv[i]);
			i++; assert(i < argc);
			depth_max = (float)atof(argv[i]);
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
		else if (!strcmp(argv[i], "-gui")) {
			gui = true;
		}
		else if (!strcmp(argv[i], "-tessellation")) {
			i++; assert(i < argc);
			theta = atoi(argv[i]);
			i++; assert(i < argc);
			phi = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud = true;
		}
		else if (!strcmp(argv[i], "-shadows")) {
			shadows = true;
		}
		else if (!strcmp(argv[i], "-bounces")) {
			i++; assert(i < argc);
			max_bounces = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight")) {
			i++; assert(i < argc);
			cutoff_weight = (float)atof(argv[i]);
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void rayTracerRender()
{
	//RayTracer::getInstance().renderRayCast();
	RayTracer::getInstance().renderRayTracing();
}

void rayTracerTracing(float x, float y)
{
	RayTracer::getInstance().renderRayDebug(x, y);
}

int main(int argc, char *argv[]) {
	char *input_file = nullptr;
	int width = 200;
	int height = 200;
	char *output_file = nullptr;
	float depth_min = -FLT_MAX;
	float depth_max = FLT_MAX;
	char *depth_file = nullptr;
	char *normal_file = nullptr;
	bool output_mode = false, depth_mode = false, normal_mode = false, shade_back = false;
	int tessellationTheta = 10, tessellationPhi = 5;
	bool gui = false, gouraud = false;
	int max_bounces = 1; float cutoff_weight = EPSILON; bool shadows = false;
	bool grid = false; int grid_nx = 1, grid_ny = 1, grid_nz = 1, grid_visualize = false;

	// 以下用于调试程序时模拟输入参数
	if (DEBUG) 
	{
		const int filename_buffer_length = 64;
		input_file = new char[filename_buffer_length];
		strcpy_s(input_file, filename_buffer_length, "Input/scene5_01_sphere.txt");
		width = 200, height = 200;
		if (DEBUG_LOG) 
		{
			width = 200, height = 200;
		}
		output_mode = true, depth_mode = false, normal_mode = false, shade_back = true;
		if (output_mode) 
		{
			output_file = new char[filename_buffer_length];
			strcpy_s(output_file, filename_buffer_length, "Output/scene5_01.tga");
		}
		if (depth_mode) 
		{
			depth_min = 9;
			depth_max = 11;
			depth_file = new char[filename_buffer_length];
			strcpy_s(depth_file, filename_buffer_length, "Output/depth4_05.tga");
		}
		if (normal_mode) 
		{
			normal_file = new char[filename_buffer_length];
			strcpy_s(normal_file, filename_buffer_length, "Output/normals4_05_2.tga");
		}
		gui = true;
		tessellationTheta = 30;
		tessellationPhi = 15;
		gouraud = true;
		max_bounces = 5;
		cutoff_weight = 0.01f;
		shadows = true;
		//grid
		grid = true;
		grid_nx = 16; grid_ny = 16; grid_nz = 16;
		grid_visualize = true;
	}
	else // 从控制台输入参数
	{
		handleParameter(argc, argv, input_file, width, height, output_file, depth_file, depth_min, depth_max, normal_file, output_mode, depth_mode, normal_mode, shade_back, tessellationTheta, tessellationPhi, gui, gouraud, max_bounces, cutoff_weight, shadows);
	}

	SceneParser *scene = new SceneParser(input_file);
	RayTracer *raytracer = &(RayTracer::getInstance());
	raytracer->initialize(width, height, scene, max_bounces, cutoff_weight, shadows);
	if (output_mode)raytracer->setOutput(output_file, shade_back);
	if (depth_mode)raytracer->setDepth(depth_file, depth_min, depth_max);
	if (normal_mode)raytracer->setNormal(normal_file);
	if (grid) raytracer->setGrid(grid_nx, grid_ny, grid_nz, grid_visualize);
	if (DEBUG_LOG)raytracer->setDebugMode(1);
	Sphere::SetTessellationParameters(tessellationTheta, tessellationPhi, gouraud);
	if (gui)
	{
		GLCanvas* canvas = new GLCanvas();
		canvas->initialize(scene, rayTracerRender, rayTracerTracing, nullptr, false);
		delete canvas;
	}
	else
	{
		raytracer->renderRayTracing();
	}

	if (DEBUG)
	{
		delete[] input_file;
		delete[] output_file;
		delete[] depth_file;
	}
	delete scene;
	//delete raytracer; 改为单例后不需要手动释放
	return 0;
}