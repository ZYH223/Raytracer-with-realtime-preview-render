#include "glCanvas.h"
#include "scene_parser.h"
#include "light.h"
#include "camera.h"
#include "group.h"
// ========================================================

// STATIC VARIABLES

// The size of window display
const int WIDTH = 800;
const int HEIGHT = 800;

// A reference to the function that performs the raytracing
// This function will get called from the 'keyboard' routine
void (*GLCanvas::renderFunction)(void);

// A pointer to the global SceneParser
SceneParser *GLCanvas::scene;

// State of the mouse cursor
int GLCanvas::mouseButton;
double GLCanvas::mouseX;
double GLCanvas::mouseY;

// ========================================================
// ========================================================

#ifdef SPECULAR_FIX
// global variable allows (hacky) communication with Material::setMaterial()
int SPECULAR_FIX_WHICH_PASS = 0;
#endif


// ========================================================
// Draw the x-, y-, and z-axes with lighting disabled
// ========================================================

void GLCanvas::drawAxes(void)
{
  glDisable(GL_LIGHTING);
  // Draw the x-axis in red
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
  glEnd();
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(1.0, 0.0,  0.0 );
    glVertex3f(0.8, 0.07, 0.0 );
    glVertex3f(0.8, 0.0,  0.07);
    glVertex3f(0.8,-0.07, 0.0 );
    glVertex3f(0.8, 0.0, -0.07);
    glVertex3f(0.8, 0.07, 0.0 );
  glEnd();

  // Draw the y-axis in green
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
  glEnd();
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f( 0.0,  1.0, 0.0 );
    glVertex3f( 0.07, 0.8, 0.0 );
    glVertex3f( 0.0,  0.8, 0.07);
    glVertex3f(-0.07, 0.8, 0.0 );
    glVertex3f( 0.0,  0.8,-0.07);
    glVertex3f( 0.07, 0.8, 0.0 );
  glEnd();

  // Draw the z-axis in blue
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
  glEnd();
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f( 0.0,  0.0,  1.0);
    glVertex3f( 0.07, 0.0,  0.8);
    glVertex3f( 0.0,  0.07, 0.8);
    glVertex3f(-0.07, 0.0,  0.8);
    glVertex3f( 0.0, -0.07, 0.8);
    glVertex3f( 0.07, 0.0,  0.8);
  glEnd();
}

// ========================================================
// Callback for the OpenGL display loop.  To request that
// this function get called, use glutPostRedisplay()
// ========================================================

void GLCanvas::display(GLFWwindow* window)
{
  // Clear the display buffer
  Vec3f bgColor = scene->getBackgroundColor();
  glClearColor(bgColor.x(), bgColor.y(), bgColor.z(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scene->getCamera()->glInit(WIDTH, HEIGHT);
  // Set the camera parameters
  glMatrixMode(GL_MODELVIEW);
  //glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  scene->getCamera()->glPlaceCamera();
  //glOrtho(-5, 5, -5, 5, -10, 10);

  // ========================================================
  // DRAW AXES
  // remove this line once you've started rendering primitive objects
  drawAxes(); 
  // ========================================================


  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  // Place each of the lights in the scene
  for (int i=0; i<scene->getNumLights(); i++) {
    scene->getLight(i)->glInit(i);
  }

#if !SPECULAR_FIX

  // DEFAULT: single pass rendering
  // Draw the scene once
  SPECULAR_FIX_WHICH_PASS = 0;
  scene->getGroup()->paint();

#else

  // OPTIONAL: 3 pass rendering to fix the specular highlight 
  // artifact for small specular exponents (wide specular lobe)

  // First pass, draw the specular highlights
  SPECULAR_FIX_WHICH_PASS = 0;
  scene->getGroup()->paint();
  
  glDepthFunc(GL_EQUAL);
  glEnable(GL_BLEND);

  // Second pass, multiply specular highlights by normal dot light
  SPECULAR_FIX_WHICH_PASS = 1;
  glBlendFunc(GL_DST_COLOR, GL_ZERO);
  scene->getGroup()->paint();
  
  // Third pass, add diffuse & ambient components
  SPECULAR_FIX_WHICH_PASS = 2;
  glBlendFunc(GL_ONE, GL_ONE);
  scene->getGroup()->paint();

  glDepthFunc(GL_LESS);
  glDisable(GL_BLEND);

#endif

  // Swap the back buffer with the front buffer to display the scene
  //glutSwapBuffers();
  //glfwSwapBuffers(window);
}

// ========================================================
// Callback function for window resize
// ========================================================

void GLCanvas::reshape(GLFWwindow* window, double w, double h) {
  // Set the OpenGL viewport to fill the entire window
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  // Set the camera parameters to reflect the changes
  scene->getCamera()->glInit(w, h);

  // OPTIONAL: If you'd also like to resize your rendering, 
  // add code to do that here
}

// ========================================================
// Callback function for mouse click or release
// ========================================================

void GLCanvas::mouse(GLFWwindow* window, int button, int action, int mods) {
  // Save the current state of the mouse.  This will be
  // used by the 'motion' function
	if (action == GLFW_PRESS)
	{
		mouseButton = button;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mouseX = x;
		mouseY = y;
	}
	else if (action == GLFW_RELEASE)
	{
		mouseButton = -1;
		mouseX = 0;
		mouseY = 0;
	}
  
}

// ========================================================
// Callback function for mouse drag
// ========================================================

void GLCanvas::motion(GLFWwindow* window, double x, double y) {
  // Left button = rotation
  // (rotate camera around the up and horizontal vectors)
  if (mouseButton == GLFW_MOUSE_BUTTON_LEFT) {
    scene->getCamera()->rotateCamera(0.005*(mouseX-x), 0.005*(mouseY-y));
    mouseX = x;
    mouseY = y;
  }
  // Middle button = translation
  // (move camera perpendicular to the direction vector)
  else if (mouseButton == GLFW_MOUSE_BUTTON_MIDDLE) {
    scene->getCamera()->truckCamera((mouseX-x)*0.05, (y-mouseY)*0.05);
    mouseX = x;
    mouseY = y;
  }
  // Right button = zoom
  // (move camera along the direction vector)
  else if (mouseButton == GLFW_MOUSE_BUTTON_RIGHT) {
    scene->getCamera()->dollyCamera((x-mouseX)*0.05);
    mouseX = x;
    mouseY = y;
  }

  // Redraw the scene with the new camera parameters
  //glutPostRedisplay();
}

// ========================================================
// Callback function for keyboard events
// ========================================================

void GLCanvas::keyboard (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)return;
	switch (key) {
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_R:
			printf("Rendering scene... "); 
			fflush(stdout);
			if (renderFunction) renderFunction();
			printf("done.\n");
			break;
		default:
			printf("UNKNOWN KEYBOARD INPUT  '%c'\n", key);
	}
}

// ========================================================
// The callback function for glfw library to get 
// framebuffer.
// ========================================================

void GLCanvas::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// ========================================================
// Initialize all appropriate OpenGL variables, set
// callback functions, and start the main event loop.
// This function will not return but can be terminated
// by calling 'exit(0)'
// ========================================================

void GLCanvas::initialize(SceneParser *_scene, void (*_renderFunction)(void)) {
  scene = _scene;
  renderFunction = _renderFunction;

  //// Set window parameters
  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  //glEnable(GL_DEPTH_TEST);
  //// OPTIONAL: If you'd like to set the window size from 
  //// the command line, do that here
  //glutInitWindowSize(400,400);
  //glutInitWindowPosition(100,100);
  //glutCreateWindow("OpenGL Viewer");

  // Rewriten with glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Viewer", NULL, NULL);
  if (window == NULL)
  {
	  std::cout << "Failed to create GLFW window" << std::endl;
	  glfwTerminate();
	  return;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
	  std::cout << "Failed to initialize GLAD" << std::endl;
	  return;
  }
  glViewport(0, 0, WIDTH, HEIGHT);

  // Set global lighting parameters
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_NORMALIZE);

  // Ambient light
  Vec3f ambColor = scene->getAmbientLight();
  GLfloat ambient[] = { ambColor.r(), ambColor.g(), ambColor.b(), 1.0 };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  mouseButton = -1;
  // Initialize callback functions
  //glutMouseFunc(mouse);
  //glutMotionFunc(motion);
  //glutDisplayFunc(display);
  //glutReshapeFunc(reshape);
  //glutKeyboardFunc(keyboard);
  //glutMainLoop();
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, keyboard);
  glfwSetMouseButtonCallback(window, mouse);
  glfwSetCursorPosCallback(window, motion);
  glfwSetScrollCallback(window, reshape);
  glfwMakeContextCurrent(window);
  while (!glfwWindowShouldClose(window))
  {
	  display(window);
	  glfwSwapBuffers(window);
	  glfwPollEvents();
  }
}

// ========================================================
// ========================================================
