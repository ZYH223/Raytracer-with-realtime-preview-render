#pragma once
#include "image.h"
#include "light.h"
#include "ray.h"
#include "hit.h"
#include "group.h"
#include "camera.h"
#include "scene_parser.h"

class RayTracer {// 实现为单例
public:
	static RayTracer& getInstance();
	//RayTracer(int width, int height, SceneParser* scene);
	~RayTracer();
	void initialize(int width, int height, SceneParser* scene);
	void setOutput(char *output_file, bool shade_back);
	void setDepth(char *depth_file, float depth_min, float depth_max);
	void setNormal(char *normal_file);
	void setDebugMode(int mode);
	void render();
private:
	RayTracer();
	int width, height;
	SceneParser *scene;
	bool output_mode; char *output_file; bool shade_back;
	bool depth_mode; char *depth_file; float depth_min; float depth_max;
	bool normal_mode; char *normal_file;
	Image *image_output, *image_depth, *image_normal;
	
	bool DEBUG_LOG;
};