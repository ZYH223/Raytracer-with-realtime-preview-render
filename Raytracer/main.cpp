#include <iostream>
#include "glCanvas.h"
#include "rayTracer.h"
using namespace std;
const bool DEBUG = true;
const bool DEBUG_LOG = false;

void handleParameter(
	int &argc, char *argv[],
	char *input_file,
	int &width, int &height,
	char *output_file,
	char *depth_file, float &depth_min, float &depth_max,
	char *normal_file,
	bool &diffuse_mode, bool &depth_mode, bool &normal_mode, bool &shade_back)
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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
}

void rayTracerRender()
{
	RayTracer::getInstance().render();
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

	// 以下用于调试程序时模拟输入参数
	if (DEBUG) 
	{
		const int filename_buffer_length = 64;
		input_file = new char[filename_buffer_length];
		strcpy_s(input_file, filename_buffer_length, "Input/scene3_01_cube_orthographic.txt");
		width = 200, height = 200;
		if (DEBUG_LOG) 
		{
			width = 32, height = 32;
		}
		output_mode = true, depth_mode = false, normal_mode = false, shade_back = false;
		if (output_mode) 
		{
			output_file = new char[filename_buffer_length];
			strcpy_s(output_file, filename_buffer_length, "Output/output3_01.tga");
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
	else // 从控制台输入参数
	{
		handleParameter(argc, argv, input_file, width, height, output_file, depth_file, depth_min, depth_max, normal_file, output_mode, depth_mode, normal_mode, shade_back);
	}

	SceneParser *scene = new SceneParser(input_file);
	RayTracer *raytracer = &(RayTracer::getInstance());
	raytracer->initialize(width, height, scene);
	if (output_mode)raytracer->setOutput(output_file, shade_back);
	if (depth_mode)raytracer->setDepth(depth_file, depth_min, depth_max);
	if (normal_mode)raytracer->setNormal(normal_file);
	if (DEBUG_LOG)raytracer->setDebugMode(1);
	GLCanvas *canvas = new GLCanvas();
	canvas->initialize(scene, rayTracerRender);

	if (DEBUG)
	{
		delete[] input_file;
		delete[] output_file;
		delete[] depth_file;
		system("pause");
	}
	delete scene;
	//delete raytracer; 改为单例后不需要手动释放
	delete canvas;
	return 0;
}