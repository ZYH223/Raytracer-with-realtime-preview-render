#include <iostream>
#include "scene_parser.h"
#include "image.h"
#include "ray.h"
#include "hit.h"
#include "Group.h"
#include "Camera.h"
using namespace std;
const bool DEBUG = false;

int main(int argc, char *argv[]) {
	char *input_file = NULL;
	int width = 100;
	int height = 100;
	char *output_file = NULL;
	float depth_min = 0;
	float depth_max = 1;
	char *depth_file = NULL;

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
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
	input_file = new char[32];
	strcpy_s(input_file, 32, "Input/scene1_04.txt");
	width = 64, height = 64;
	output_file = new char[32];
	strcpy_s(output_file, 32, "Output/output1_04.tga");
	depth_min = 12;
	depth_max = 17;
	depth_file = new char[32];
	strcpy_s(depth_file, 32, "Output/depth1_04.tga");

	SceneParser scene(input_file);
	Image image(width, height);
	Image image_depth(width, height);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Vec3f color = scene.getBackgroundColor();
			Ray r = scene.getCamera()->generateRay(Vec2f(i / (float)width, j / (float)height));
			Hit h(depth_max, new Material(color));
			if (DEBUG)cout << Vec3f(i, j, 0) << ":" << r << " intersect with ";
			if (scene.getGroup()->intersect(r, h, depth_min, depth_max))
			{
				if (DEBUG)cout << h.getIntersectionPoint() << " and return " << h.getMaterial()->getDiffuseColor() << endl;
				//if (DEBUG)cout << h << endl;
				color = h.getMaterial()->getDiffuseColor();
			}
			else
			{
				if (DEBUG)cout << "nothing" << endl;
			}
			image.SetPixel(i, j, color);
			float depth = 1.0f - (h.getT() - depth_min) / (depth_max - depth_min);
			image_depth.SetPixel(i, j, Vec3f(depth, depth, depth));
		}
	}
	image.SaveTGA(output_file);
	image_depth.SaveTGA(depth_file);

	delete[] input_file;
	delete[] output_file;
	delete[] depth_file;
	system("pause");
	return 0;
}