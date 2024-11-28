#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>

#include "render/shader.h"
#include "Camera/Camera.h"
#include <SkyBox.h>
#include <objloader.h>
#include <TextureLoader.h>


static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
static 	bool firstMouse = true;

int ScreenSizeX;
int ScreenSizeY;

float xpos;
float ypos;
float lastX;
float lastY;
const float sensitivity = 0.1f;

float one_third = 1./3.;
float two_third = 2./3.;

// View control
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static glm::vec3 eye_center = glm::vec3(500,0,500);;
static float cameraMovementSpeed = 0.f;
glm::float32 FoV = 60;
glm::float32 zNear = 0.1f;
glm::float32 zFar = 6000.0f;
Camera camera = Camera(eye_center,viewAzimuth,viewPolar,FoV,zNear,zFar,cameraMovementSpeed);
SkyBox b;
TextureLoader textureLoader;



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	//window = glfwCreateWindow(ScreenSizeX, ScreenSizeY, "Lab 2", NULL, nullptr);
	ScreenSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	ScreenSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	window =   glfwCreateWindow(1024,768, "Project", nullptr	, nullptr);

	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	float deltaTime = 0.0f; // Time between current frame and last frame
	b.initialize(camera.get_position(), glm::vec3(3000, 3000, 3000),textureLoader);
	Object o;

	o.initialize(glm::vec3(0.,0.,0.), glm::vec3(100.,100.,100.),textureLoader); //glm::vec3(150, 150, 150)

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render the SkyBox
		b.render(camera.get_MVP());

		float dist= length(abs(o.get_position()-camera.get_position()));
		if(dist<500) {
			o.set_scale(glm::vec3(100,100,100));
		}
		else if(dist>=500) {
			o.set_scale(max(glm::vec3(450-0.5*dist,450-0.5*dist,450-0.5*dist),glm::vec3(0.f,0.f,0.f)));
		}

		o.render(camera.get_MVP());

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		camera.set_speed(100.f*deltaTime);

		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Project | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	b.cleanup();
	o.cleanup();
	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivityX = 0.001;
	float sensitivityY = 0.001;
	xoffset *= sensitivityX;
	yoffset *= sensitivityY;
	camera.mouse_callback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	std::cout<<cameraMovementSpeed<<std::endl;
	if (key == GLFW_KEY_R && glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS) {
		std::cout << "Reset." << std::endl;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else {
		camera.key_callback(window, key, scancode, action, mode);
		b.move(camera.get_position());
	}
}
