// ====================================================================
// GLCanvas class by Rob Jagnow.
//
// The GLCanvas can be created with a call to the 'initialize' routine,
// which takes as parameters a pointer to the global SceneParser and
// the name of the routine that will perform the ray tracing.  Once the
// OpenGL interface is open, the scene can be rendered from the current
// camera position by pressing the 'r' key.
// ====================================================================

#ifndef _GL_CANVAS_H_
#define _GL_CANVAS_H_

#include <stdlib.h>
#include "constant.h"
#include <GLFW/glfw3.h>
#include "grid.h"

class SceneParser;
//class Grid;

// ====================================================================
// OPTIONAL: 3 pass rendering to fix the specular highlight 
// artifact for small specular exponents (wide specular lobe)

//   0: don't fix   
//   1: do fix
#define SPECULAR_FIX 0 

// ====================================================================

class GLCanvas {

private:
  // A reference to the function that performs the raytracing
  // This gets called from the 'keyboard' routine
  static void (*renderFunction)(void);

  // A reference to the function that traces the ray tree for a single pixel
  // This gets called from the 'keyboard' routine
  static void (*traceRayFunction)(float,float);

  // A pointer to the global SceneParser
  static SceneParser *scene;

  // A pointer to the grid
  static Grid *grid;
  static bool visualize_grid;
  static int visualize_grid_march;

  // State of the mouse cursor
  static int mouseButton;
  static double mouseX;
  static double mouseY;

  // Helper function for the display routine
  static void drawAxes(void);

  // Callback functions for mouse and keyboard events
  static void display(GLFWwindow* window);
  static void reshape(GLFWwindow* window, double w, double h);
  static void mouse(GLFWwindow* window, int button, int action, int mods);
  static void motion(GLFWwindow* window, double x, double y);
  static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

public:
  // Constructor and destructor
  GLCanvas(void) {
    renderFunction = NULL;
    traceRayFunction = NULL; }
 ~GLCanvas(void) { }

  // Set up the canvas and enter the rendering loop
  // Note that this function will not return but can be
  // terminated by calling 'exit(0)'

  void initialize(SceneParser *_scene, 
		  void (*_renderFunction)(void), 
		  void (*_traceRayFunction)(float, float),
		  Grid *_grid, bool _visualize_grid);
};

// ====================================================================

#endif
