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
	bool &gui, bool &gouraud)
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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void rayTracerRender()
{
	RayTracer::getInstance().renderRayCast();
}

void rayTracerTracing(float x, float y)
{
	//RayTracer::getInstance().traceRay();
}

int main(int argc, char *argv[]) {
	char *input_file = nullptr;
	int width = 100;
	int height = 100;
	char *output_file = nullptr;
	float depth_min = -FLT_MAX;
	float depth_max = FLT_MAX;
	char *depth_file = nullptr;
	char *normal_file = nullptr;
	bool output_mode = false, depth_mode = false, normal_mode = false, shade_back = false;
	int tessellationTheta = 10, tessellationPhi = 5;
	bool gui = false, gouraud = false;

	// 以下用于调试程序时模拟输入参数
	if (DEBUG) 
	{
		const int filename_buffer_length = 64;
		input_file = new char[filename_buffer_length];
		strcpy_s(input_file, filename_buffer_length, "Input/scene4_01_sphere_shadow.txt");
		width = 300, height = 300;
		if (DEBUG_LOG) 
		{
			width = 32, height = 32;
		}
		output_mode = true, depth_mode = false, normal_mode = false, shade_back = false;
		if (output_mode) 
		{
			output_file = new char[filename_buffer_length];
			strcpy_s(output_file, filename_buffer_length, "Output/scene4_01.tga");
		}
		if (depth_mode) 
		{
			depth_min = 9;
			depth_max = 11;
			depth_file = new char[filename_buffer_length];
			strcpy_s(depth_file, filename_buffer_length, "Output/depth.tga");
		}
		if (normal_mode) 
		{
			normal_file = new char[filename_buffer_length];
			strcpy_s(normal_file, filename_buffer_length, "Output/normals.tga");
		}
		gui = false;
		tessellationTheta = 10;
		tessellationPhi = 5;
		gouraud = false;
	}
	else // 从控制台输入参数
	{
		handleParameter(argc, argv, input_file, width, height, output_file, depth_file, depth_min, depth_max, normal_file, output_mode, depth_mode, normal_mode, shade_back, tessellationTheta, tessellationPhi, gui, gouraud);
	}

	SceneParser *scene = new SceneParser(input_file);
	RayTracer *raytracer = &(RayTracer::getInstance());
	raytracer->initialize(width, height, scene, 100, 10.0f, true);
	if (output_mode)raytracer->setOutput(output_file, shade_back);
	if (depth_mode)raytracer->setDepth(depth_file, depth_min, depth_max);
	if (normal_mode)raytracer->setNormal(normal_file);
	if (DEBUG_LOG)raytracer->setDebugMode(1);
	Sphere::SetTessellationParameters(tessellationTheta, tessellationPhi, gouraud);
	if (gui)
	{
		GLCanvas* canvas = new GLCanvas();
		canvas->initialize(scene, rayTracerRender, rayTracerTracing);
		delete canvas;
	}
	else
	{
		raytracer->renderRayCast();
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